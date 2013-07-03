#include "solver_nr.h"
#include "output.h"

// Note: due to extensive indentation, line lengths are increased from 80 to 120 characters.

#define MT // this enables multithreaded SuperLU

// #define NR_MATRIX_OUT
// This directive enables a text file dump of the sparse-formatted admittance matrix - useful for debugging

#ifdef MT
#include <SuperLU/SRC/pdsp_defs.h> // superLU_MT
#else
#include <slu_ddefs.h> // Sequential superLU (other platforms)
#endif

namespace SmartGridToolbox
{

   void merge_sort(Y_NR *Input_Array, unsigned int Alen, Y_NR *Work_Array)
   {
      // Merge sorting algorithm basis stolen from auction.cpp in market module.
      unsigned int split_point;
      unsigned int right_length;
      Y_NR *leftside, *rightside;
      Y_NR *Final_P;

      if (Alen>0) // Only occurs if over zero
      {
         split_point = Alen/2;
         // Find the middle point
         right_length = Alen - split_point;
         // Figure out how big the right hand side is

         // Make the appropriate pointers
         leftside = Input_Array;
         rightside = Input_Array+split_point;

         // Check to see what condition we are in (keep splitting it)
         if (split_point>1)
            merge_sort(leftside,split_point,Work_Array);

         if (right_length>1)
            merge_sort(rightside,right_length,Work_Array);

         // Point at the first location
         Final_P = Work_Array;

         // Merge them now
         do {
            if (leftside->col_ind < rightside->col_ind)
               *Final_P++ = *leftside++;
            else if (leftside->col_ind == rightside->col_ind)
            {
               if (leftside->row_ind < rightside->row_ind)
                  *Final_P++ = *leftside++;
               else if (leftside->row_ind > rightside->row_ind)
                  *Final_P++ = *rightside++;
               else // Catch for duplicate entries
               {
                  error("NR: duplicate entry found in admittance matrix. Look for parallel lines!");
                  /*  TROUBLESHOOT
                      While sorting the admittance matrix for the Newton-Raphson solver, a duplicate entry was found.
                      This is usually caused by a line between two nodes having another, parallel line between the
                      same two nodes.  This is only an issue if the parallel lines overlap in phase (e.g., AB and BC).
                      If no overlapping phase is present, this error should not occur.  Methods to narrow down the
                      location of this conflict are under development. */
               }
            }
            else
               *Final_P++ = *rightside++;
         } while ((leftside<(Input_Array+split_point)) && (rightside<(Input_Array+Alen)));
         // Sort the list until one of them empties

         while (leftside<(Input_Array+split_point))
            *Final_P++ = *leftside++;
         // Put any remaining entries into list.

         while (rightside<(Input_Array+Alen))
            *Final_P++ = *rightside++;
         // Put any remaining entries into list.

         memcpy(Input_Array,Work_Array,sizeof(Y_NR)*Alen);
         // Copy the result back into the input
      } // End length > 0
   }

   void buildAdmit(unsigned int bus_count, BUSDATA *bus, unsigned int branch_count, BRANCHDATA *branch,
                   unsigned int max_total_variables, unsigned int & total_variables,
                   Bus_admit * & BA_diag, 
                   unsigned int & size_offdiag_PQ, Y_NR * & Y_offdiag_PQ, 
                   unsigned int & size_diag_fixed, Y_NR * & Y_diag_fixed, 
                   bool & NR_realloc_needed)
   {
      static unsigned int max_size_offdiag_PQ, max_size_diag_fixed;

      // Miscellaneous index variable
      unsigned int indexer, tempa, tempb, jindexer, kindexer;
      char jindex, kindex;
      char temp_index, temp_index_b;
      unsigned int temp_index_c;

      //Miscellaneous counter tracker
      unsigned int index_count = 0;

      //Temporary size variable
      char temp_size, temp_size_b, temp_size_c;

      //Phase collapser variable
      unsigned char phase_worka, phase_workb, phase_workc, phase_workd, phase_worke;

      //Working matrix for admittance collapsing/determinations
      Complex tempY[3][3];

      //Miscellaneous flag variables
      bool Full_Mat_A, Full_Mat_B, proceed_flag;

      // Temporary admittance variables
      Complex Temp_Ad_A[3][3];
      Complex Temp_Ad_B[3][3];

      //Build the diagnoal elements of the bus admittance matrix - this should only happen once no matter what
      if (BA_diag == NULL)
      {
         BA_diag = (Bus_admit *)malloc(bus_count *sizeof(Bus_admit));
         // BA_diag store the location and value of diagonal elements of Bus Admittance matrix

         //Make sure it worked
         if (BA_diag == NULL)
         {
            error("NR: Failed to allocate memory for one of the necessary matrices");
            /*  TROUBLESHOOT
                During the allocation stage of the NR algorithm, one of the matrices failed to be allocated.
                Please try again and if this bug persists, submit your code and a bug report using the trac
                website.
                */
         }
      }

      for (indexer=0; indexer<bus_count; indexer++) // Construct the diagonal elements of Bus admittance matrix.
      {
         //Determine the size we need
         phase_worka = 0;
         for (jindex=0; jindex<3; jindex++) //Accumulate number of phases
         {
            phase_worka += ((bus[indexer].phases & (0x01 << jindex)) >> jindex);
         }
         BA_diag[indexer].size = phase_worka;

         //Ensure the admittance matrix is zeroed
         for (jindex=0; jindex<3; jindex++)
         {
            for (kindex=0; kindex<3; kindex++)
            {
               BA_diag[indexer].Y[jindex][kindex] = 0;
               tempY[jindex][kindex] = 0;
            }
         }

         //Now go through all of the branches to get the self admittance information (hinges on size)
         for (kindexer=0; kindexer<(bus[indexer].Link_Table.size());kindexer++)
         {
            //Assign jindexer as intermediate variable (easier for me this way)
            jindexer = bus[indexer].Link_Table[kindexer];

            if ((branch[jindexer].from == indexer) || (branch[jindexer].to == indexer))
               // Bus is the from or to side of things - not sure how it would be in link table otherwise, but meh
            {
               if ((bus[indexer].phases & 0x07) == 0x07) //Full three phase
               {
                  for (jindex=0; jindex<3; jindex++) //Add in all three phase values
                  {
                     //See if this phase is valid
                     phase_workb = 0x04 >> jindex;

                     if ((phase_workb & branch[jindexer].phases) == phase_workb)
                     {
                        for (kindex=0; kindex<3; kindex++)
                        {
                           //Check phase
                           phase_workd = 0x04 >> kindex;

                           if ((phase_workd & branch[jindexer].phases) == phase_workd)
                           {
                              if (branch[jindexer].from == indexer) //We're the from version
                              {
                                 tempY[jindex][kindex] += branch[jindexer].YSfrom[jindex*3+kindex];
                              }
                              else //Must be the to version
                              {
                                 tempY[jindex][kindex] += branch[jindexer].YSto[jindex*3+kindex];
                              }
                           } //End valid column phase
                        }
                     } //End valid row phase
                  }
               }
               else
               {
                  // We must be a single or two-phase line - always populate the upper left portion of matrix
                  // (easier for later)
                  switch(bus[indexer].phases & 0x07) {
                     case 0x00: //No phases (we've been faulted out
                        {
                           break; //Just get us outta here
                        }
                     case 0x01: //Only C
                        {
                           if ((branch[jindexer].phases & 0x01) == 0x01) //Phase C valid on branch
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[8];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[8];
                              }
                           } //End valid phase C
                           break;
                        }
                     case 0x02: //Only B
                        {
                           if ((branch[jindexer].phases & 0x02) == 0x02) //Phase B valid on branch
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[4];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[4];
                              }
                           } //End valid phase B
                           break;
                        }
                     case 0x03: //B & C
                        {
                           phase_worka = (branch[jindexer].phases & 0x03); //Extract branch phases

                           if (phase_worka == 0x03) //Full B & C
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[4];
                                 tempY[0][1] += branch[jindexer].YSfrom[5];
                                 tempY[1][0] += branch[jindexer].YSfrom[7];
                                 tempY[1][1] += branch[jindexer].YSfrom[8];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[4];
                                 tempY[0][1] += branch[jindexer].YSto[5];
                                 tempY[1][0] += branch[jindexer].YSto[7];
                                 tempY[1][1] += branch[jindexer].YSto[8];
                              }
                           } //End valid B & C
                           else if (phase_worka == 0x01) //Only C branch
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[1][1] += branch[jindexer].YSfrom[8];
                              }
                              else //To branch
                              {
                                 tempY[1][1] += branch[jindexer].YSto[8];
                              }
                           } //end valid C
                           else if (phase_worka == 0x02) //Only B branch
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[4];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[4];
                              }
                           } //end valid B
                           else //Must be nothing then - all phases must be faulted, or something
                              ;
                           break;
                        }
                     case 0x04: //Only A
                        {
                           if ((branch[jindexer].phases & 0x04) == 0x04) //Phase A is valid
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[0];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[0];
                              }
                           } //end valid phase A
                           break;
                        }
                     case 0x05: //A & C
                        {
                           phase_worka = branch[jindexer].phases & 0x05; //Extract phases

                           if (phase_worka == 0x05) //Both A & C valid
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[0];
                                 tempY[0][1] += branch[jindexer].YSfrom[2];
                                 tempY[1][0] += branch[jindexer].YSfrom[6];
                                 tempY[1][1] += branch[jindexer].YSfrom[8];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[0];
                                 tempY[0][1] += branch[jindexer].YSto[2];
                                 tempY[1][0] += branch[jindexer].YSto[6];
                                 tempY[1][1] += branch[jindexer].YSto[8];
                              }
                           } //End A & C valid
                           else if (phase_worka == 0x04) //Only A valid
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[0];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[0];
                              }
                           } //end only A valid
                           else if (phase_worka == 0x01) //Only C valid
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[1][1] += branch[jindexer].YSfrom[8];
                              }
                              else //To branch
                              {
                                 tempY[1][1] += branch[jindexer].YSto[8];
                              }
                           } //end only C valid
                           else //No connection - must be faulted
                              ;
                           break;
                        }
                     case 0x06: //A & B
                        {
                           phase_worka = branch[jindexer].phases & 0x06; //Extract phases

                           if (phase_worka == 0x06) //Valid A & B phase
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[0];
                                 tempY[0][1] += branch[jindexer].YSfrom[1];
                                 tempY[1][0] += branch[jindexer].YSfrom[3];
                                 tempY[1][1] += branch[jindexer].YSfrom[4];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[0];
                                 tempY[0][1] += branch[jindexer].YSto[1];
                                 tempY[1][0] += branch[jindexer].YSto[3];
                                 tempY[1][1] += branch[jindexer].YSto[4];
                              }
                           } //End valid A & B
                           else if (phase_worka == 0x04) //Only valid A
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[0][0] += branch[jindexer].YSfrom[0];
                              }
                              else //To branch
                              {
                                 tempY[0][0] += branch[jindexer].YSto[0];
                              }
                           } //end valid A
                           else if (phase_worka == 0x02) //Only valid B
                           {
                              if (branch[jindexer].from == indexer) //From branch
                              {
                                 tempY[1][1] += branch[jindexer].YSfrom[4];
                              }
                              else //To branch
                              {
                                 tempY[1][1] += branch[jindexer].YSto[4];
                              }
                           } //end valid B
                           else //Default - must be already handled
                              ;
                           break;
                        }
                     default: //How'd we get here?
                        {
                           error("Unknown phase connection in NR self admittance diagonal");
                           /*  TROUBLESHOOT
                               An unknown phase condition was encountered in the NR solver when constructing
                               the self admittance diagonal.  Please report this bug and submit your code to
                               the trac system.
                               */
                           break;
                        }
                  } //switch end
               } //1 or 2 phase end
            } //phase accumulation end
            else //It's nothing (no connnection)
               ;
         } //branch traversion end

         //Store the self admittance into BA_diag.  Also update the indices for possible use later
         BA_diag[indexer].col_ind = BA_diag[indexer].row_ind = index_count;
         // Store the row and column starting information (square matrices)
         bus[indexer].Matrix_Loc = index_count; // Store our location so we know where we go
         index_count += BA_diag[indexer].size;
         // Update the index for this matrix's size, so next one is in appropriate place


         //Store the admittance values into the BA_diag matrix structure
         for (jindex=0; jindex<BA_diag[indexer].size; jindex++)
         {
            for (kindex=0; kindex<BA_diag[indexer].size; kindex++)
               //Store values - assume square matrix - don't bother parsing what doesn't exist.
            {
               BA_diag[indexer].Y[jindex][kindex] = tempY[jindex][kindex]; // Store the self admittance terms.
            }
         }
      } //End diagonal construction

      //Store the size of the diagonal, since it represents how many variables we are solving (useful later)
      total_variables=index_count;

      //Check to see if we've exceeded our max.  If so, reallocate!
      if (total_variables > max_total_variables)
         NR_realloc_needed = true;

      // Build the off_diagonal_PQ bus elements of 6n*6n Y_NR matrix.Equation (12). All the value in this part
      // will not be updated at each iteration.
      // Constructed using sparse methodology, non-zero elements are the only thing considered (and non-PV)
      // No longer necessarily 6n*6n any more either,
      size_offdiag_PQ = 0;
      for (jindexer=0; jindexer<branch_count;jindexer++) //Parse all of the branches
      {
         tempa  = branch[jindexer].from;
         tempb  = branch[jindexer].to;

         //Preliminary check to make sure we weren't missed in the initialization
         if ((bus[tempa].Matrix_Loc == -1) || (bus[tempb].Matrix_Loc == -1))
         {
            error("An element in NR line:%d was not properly localized");
            /*  TROUBLESHOOT
                When parsing the bus list, the Newton-Raphson solver found a bus that did not
                appear to have a location within the overall admittance/Jacobian matrix.  Please
                submit this as a bug with your code on the Trac site.
                */
         }

         for (jindex=0; jindex<3; jindex++) //rows
         {
            //See if this phase is valid
            phase_workb = 0x04 >> jindex;

            if ((phase_workb & branch[jindexer].phases) == phase_workb) //Row check
            {
               for (kindex=0; kindex<3; kindex++) //columns
               {
                  //Check this phase as well
                  phase_workd = 0x04 >> kindex;

                  if ((phase_workd & branch[jindexer].phases) == phase_workd) //Column validity check
                  {
                     if ((real(branch[jindexer].Yfrom[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                           (bus[tempb].type != BusType::PV))
                        size_offdiag_PQ += 1;

                     if ((real(branch[jindexer].Yto[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                           (bus[tempb].type != BusType::PV))
                        size_offdiag_PQ += 1;

                     if ((imag(branch[jindexer].Yfrom[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                           (bus[tempb].type != BusType::PV))
                        size_offdiag_PQ += 1;

                     if ((imag(branch[jindexer].Yto[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                           (bus[tempb].type != BusType::PV))
                        size_offdiag_PQ += 1;
                  } //end column validity check
               } //end columns of 3 phase
            } //End row validity check
         } //end rows of 3 phase
      } //end line traversion

      //Allocate the space - double the number found (each element goes in two places)
      if (Y_offdiag_PQ == NULL)
      {
         Y_offdiag_PQ = (Y_NR *)malloc((size_offdiag_PQ*2) *sizeof(Y_NR));
         // Y_offdiag_PQ store the row,column and value of off_diagonal elements of Bus Admittance matrix in
         // which all the buses are not PV buses.

         //Make sure it worked
         if (Y_offdiag_PQ == NULL)
            error("NR: Failed to allocate memory for one of the necessary matrices");

         //Save our size
         max_size_offdiag_PQ = size_offdiag_PQ;
         // Don't care about the 2x, since we'll be comparing it against itself
      }
      else if (size_offdiag_PQ > max_size_offdiag_PQ) //Something changed and we are bigger!!
      {
         //Destroy us!
         free(Y_offdiag_PQ);

         //Rebuild us, we have the technology
         Y_offdiag_PQ = (Y_NR *)malloc((size_offdiag_PQ*2) *sizeof(Y_NR));

         //Make sure it worked
         if (Y_offdiag_PQ == NULL)
            error("NR: Failed to allocate memory for one of the necessary matrices");

         //Store the new size
         max_size_offdiag_PQ = size_offdiag_PQ;

         //Flag for a reallocation
         NR_realloc_needed = true;
      }

      indexer = 0;
      for (jindexer=0; jindexer<branch_count;jindexer++) //Parse through all of the branches
      {
         //Extract both ends
         tempa  = branch[jindexer].from;
         tempb  = branch[jindexer].to;

         phase_worka = 0;
         phase_workb = 0;
         for (jindex=0; jindex<3; jindex++) //Accumulate number of phases
         {
            phase_worka += ((bus[tempa].phases & (0x01 << jindex)) >> jindex);
            phase_workb += ((bus[tempb].phases & (0x01 << jindex)) >> jindex);
         }

         if ((phase_worka==3) && (phase_workb==3)) //Both ends are full three phase, normal operations
         {
            for (jindex=0; jindex<3; jindex++) //Loop through rows of admittance matrices
            {
               //See if this row is valid for this branch
               phase_workd = 0x04 >> jindex;

               if ((branch[jindexer].phases & phase_workd) == phase_workd) //Validity check
               {
                  for (kindex=0; kindex<3; kindex++) //Loop through columns of admittance matrices
                  {
                     //Extract column information
                     phase_worke = 0x04 >> kindex;

                     if ((branch[jindexer].phases & phase_worke) == phase_worke) //Valid column too!
                     {
                        //Indices counted out from Self admittance above.  needs doubling due to complex separation
                        if ((imag(branch[jindexer].Yfrom[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                              (bus[tempb].type != BusType::PV)) //From imags
                        {
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + jindex;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + kindex;
                           Y_offdiag_PQ[indexer].Y_value = -(imag(branch[jindexer].Yfrom[jindex*3+kindex]));
                           indexer += 1;
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + jindex + 3;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + kindex + 3;
                           Y_offdiag_PQ[indexer].Y_value = imag(branch[jindexer].Yfrom[jindex*3+kindex]);
                           indexer += 1;
                        }

                        if ((imag(branch[jindexer].Yto[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                              (bus[tempb].type != BusType::PV)) //To imags
                        {
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + jindex;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + kindex;
                           Y_offdiag_PQ[indexer].Y_value = -(imag(branch[jindexer].Yto[jindex*3+kindex]));
                           indexer += 1;
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + jindex + 3;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + kindex + 3;
                           Y_offdiag_PQ[indexer].Y_value = imag(branch[jindexer].Yto[jindex*3+kindex]);
                           indexer += 1;
                        }

                        if ((real(branch[jindexer].Yfrom[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                              (bus[tempb].type != BusType::PV)) //From reals
                        {
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + jindex + 3;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + kindex;
                           Y_offdiag_PQ[indexer].Y_value = -(real(branch[jindexer].Yfrom[jindex*3+kindex]));
                           indexer += 1;
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + jindex;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + kindex + 3;
                           Y_offdiag_PQ[indexer].Y_value = -(real(branch[jindexer].Yfrom[jindex*3+kindex]));
                           indexer += 1;
                        }

                        if ((real(branch[jindexer].Yto[jindex*3+kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                              (bus[tempb].type != BusType::PV)) //To reals
                        {
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + jindex + 3;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + kindex;
                           Y_offdiag_PQ[indexer].Y_value = -(real(branch[jindexer].Yto[jindex*3+kindex]));
                           indexer += 1;
                           Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + jindex;
                           Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + kindex + 3;
                           Y_offdiag_PQ[indexer].Y_value = -(real(branch[jindexer].Yto[jindex*3+kindex]));
                           indexer += 1;
                        }
                     } //End valid column
                  } //column end
               } //End valid row
            } //row end
         } //if all 3 end
         //Clear working variables, just in case
         temp_index = temp_index_b = -1;
         temp_size = temp_size_b = temp_size_c = -1;
         Full_Mat_A = Full_Mat_B = false;

         //Intermediate store the admittance matrices so they can be directly indexed later
         switch(branch[jindexer].phases & 0x07) {
            case 0x00: //No phases (open switch or reliability excluded item)
               {
                  temp_size_c = -99; //Arbitrary flag
                  break;
               }
            case 0x01: //C only
               {
                  Temp_Ad_A[0][0] = branch[jindexer].Yfrom[8];
                  Temp_Ad_B[0][0] = branch[jindexer].Yto[8];
                  temp_size_c = 1;
                  break;
               }
            case 0x02: //B only
               {
                  Temp_Ad_A[0][0] = branch[jindexer].Yfrom[4];
                  Temp_Ad_B[0][0] = branch[jindexer].Yto[4];
                  temp_size_c = 1;
                  break;
               }
            case 0x03: //BC only
               {
                  Temp_Ad_A[0][0] = branch[jindexer].Yfrom[4];
                  Temp_Ad_A[0][1] = branch[jindexer].Yfrom[5];
                  Temp_Ad_A[1][0] = branch[jindexer].Yfrom[7];
                  Temp_Ad_A[1][1] = branch[jindexer].Yfrom[8];

                  Temp_Ad_B[0][0] = branch[jindexer].Yto[4];
                  Temp_Ad_B[0][1] = branch[jindexer].Yto[5];
                  Temp_Ad_B[1][0] = branch[jindexer].Yto[7];
                  Temp_Ad_B[1][1] = branch[jindexer].Yto[8];

                  temp_size_c = 2;
                  break;
               }
            case 0x04: //A only
               {
                  Temp_Ad_A[0][0] = branch[jindexer].Yfrom[0];
                  Temp_Ad_B[0][0] = branch[jindexer].Yto[0];
                  temp_size_c = 1;
                  break;
               }
            case 0x05: //AC only
               {
                  Temp_Ad_A[0][0] = branch[jindexer].Yfrom[0];
                  Temp_Ad_A[0][1] = branch[jindexer].Yfrom[2];
                  Temp_Ad_A[1][0] = branch[jindexer].Yfrom[6];
                  Temp_Ad_A[1][1] = branch[jindexer].Yfrom[8];

                  Temp_Ad_B[0][0] = branch[jindexer].Yto[0];
                  Temp_Ad_B[0][1] = branch[jindexer].Yto[2];
                  Temp_Ad_B[1][0] = branch[jindexer].Yto[6];
                  Temp_Ad_B[1][1] = branch[jindexer].Yto[8];

                  temp_size_c = 2;
                  break;
               }
            case 0x06: //AB only
               {
                  Temp_Ad_A[0][0] = branch[jindexer].Yfrom[0];
                  Temp_Ad_A[0][1] = branch[jindexer].Yfrom[1];
                  Temp_Ad_A[1][0] = branch[jindexer].Yfrom[3];
                  Temp_Ad_A[1][1] = branch[jindexer].Yfrom[4];

                  Temp_Ad_B[0][0] = branch[jindexer].Yto[0];
                  Temp_Ad_B[0][1] = branch[jindexer].Yto[1];
                  Temp_Ad_B[1][0] = branch[jindexer].Yto[3];
                  Temp_Ad_B[1][1] = branch[jindexer].Yto[4];

                  temp_size_c = 2;
                  break;
               }
            default:
               {
                  break;
               }
         } //end line switch/case

         if (temp_size_c==-99)
         {
            continue; //Next iteration of branch loop
         }

         if (temp_size_c==-1) //Make sure it is right
         {
            error("NR: A line's phase was flagged as not full three-phase, but wasn't");
            /*  TROUBLESHOOT
                A line inside the powerflow model was flagged as not being full three-phase or
                triplex in any form.  It failed the other cases though, so it must have been.
                Please submit your code and a bug report to the trac website.
                */
         }

         //Check the from side and get all appropriate offsets
         switch(bus[tempa].phases & 0x07) {
            case 0x01: //C
               {
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_size = 1; //Single size
                     temp_index = 0; //No offset (only 1 big)
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                     /*  TROUBLESHOOT
                         One of the lines in the powerflow model has an invalid phase in
                         reference to its to and from ends.  This should have been caught
                         earlier, so submit your code and a bug report using the trac website.
                         */
                  }
                  break;
               } //end 0x01
            case 0x02: //B
               {
                  if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_size = 1; //Single size
                     temp_index = 0; //No offset (only 1 big)
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x02
            case 0x03: //BC
               {
                  temp_size = 2; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_index = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_index = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x03) //BC
                  {
                     temp_index = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x03
            case 0x04: //A
               {
                  if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_size = 1; //Single size
                     temp_index = 0; //No offset (only 1 big)
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x04
            case 0x05: //AC
               {
                  temp_size = 2; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_index = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_index = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x05) //AC
                  {
                     temp_index = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x05
            case 0x06: //AB
               {
                  temp_size = 2; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_index = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_index = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x06) //AB
                  {
                     temp_index = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x06
            case 0x07: //ABC
               {
                  temp_size = 3; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_index = 2; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_index = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x03) //BC
                  {
                     temp_index = 1;
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_index = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x05) //AC
                  {
                     temp_index = 0;
                     Full_Mat_A = true; //Flag so we know C needs to be gapped
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x06) //AB
                  {
                     temp_index = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x07
            default:
               {
                  break;
               }
         } //End switch/case for from

         //Check the to side and get all appropriate offsets
         switch(bus[tempb].phases & 0x07) {
            case 0x01: //C
               {
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_size_b = 1; //Single size
                     temp_index_b = 0; //No offset (only 1 big)
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                     /*  TROUBLESHOOT
                         One of the lines in the powerflow model has an invalid phase in
                         reference to its to and from ends.  This should have been caught
                         earlier, so submit your code and a bug report using the trac website.
                         */
                  }
                  break;
               } //end 0x01
            case 0x02: //B
               {
                  if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_size_b = 1; //Single size
                     temp_index_b = 0; //No offset (only 1 big)
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x02
            case 0x03: //BC
               {
                  temp_size_b = 2; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_index_b = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_index_b = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x03) //BC
                  {
                     temp_index_b = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x03
            case 0x04: //A
               {
                  if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_size_b = 1; //Single size
                     temp_index_b = 0; //No offset (only 1 big)
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x04
            case 0x05: //AC
               {
                  temp_size_b = 2; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_index_b = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_index_b = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x05) //AC
                  {
                     temp_index_b = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x05
            case 0x06: //AB
               {
                  temp_size_b = 2; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_index_b = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_index_b = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x06) //AB
                  {
                     temp_index_b = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x06
            case 0x07: //ABC
               {
                  temp_size_b = 3; //Size of this matrix's admittance
                  if ((branch[jindexer].phases & 0x07) == 0x01) //C
                  {
                     temp_index_b = 2; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x02) //B
                  {
                     temp_index_b = 1; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x03) //BC
                  {
                     temp_index_b = 1;
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x04) //A
                  {
                     temp_index_b = 0; //offset
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x05) //AC
                  {
                     temp_index_b = 0;
                     Full_Mat_B = true; //Flag so we know C needs to be gapped
                  }
                  else if ((branch[jindexer].phases & 0x07) == 0x06) //AB
                  {
                     temp_index_b = 0;
                  }
                  else
                  {
                     error("NR: One of the lines has invalid phase parameters");
                  }
                  break;
               } //end 0x07
            default:
               {
                  break;
               }
         } //End switch/case for to

         //Make sure everything was set before proceeding
         if ((temp_index==-1) || (temp_index_b==-1) || (temp_size==-1) || (temp_size_b==-1) || (temp_size_c==-1))
            error("NR: Failure to construct single/double phase line indices");
         /*  TROUBLESHOOT
             A single or double phase line (e.g., just A or AB) has failed to properly initialize all of the
             indices necessary to form the admittance matrix.  Please submit a bug report, with your code, to
             the trac site.
             */

         if (Full_Mat_A) //From side is a full ABC and we have AC
         {
            for (jindex=0; jindex<temp_size_c; jindex++) //Loop through rows of admittance matrices
            {
               for (kindex=0; kindex<temp_size_c; kindex++) //Loop through columns of admittance matrices
               {
                  //Indices counted out from Self admittance above.  needs doubling due to complex separation
                  if ((imag(Temp_Ad_A[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //From imags
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex*2;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(imag(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex*2 + temp_size;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].Y_value = (imag(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;
                  }

                  if ((imag(Temp_Ad_B[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                        (bus[tempb].type != BusType::PV)) // To imags
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex*2;
                     Y_offdiag_PQ[indexer].Y_value = -(imag(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex*2 + temp_size;
                     Y_offdiag_PQ[indexer].Y_value = imag(Temp_Ad_B[jindex][kindex]);
                     indexer += 1;
                  }

                  if ((real(Temp_Ad_A[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //From reals
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex*2 + temp_size;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex*2;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;
                  }

                  if ((real(Temp_Ad_B[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //To reals
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex*2;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex*2 + temp_size;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;
                  }
               } //column end
            } //row end
         } //end full ABC for from AC

         if (Full_Mat_B) //To side is a full ABC and we have AC
         {
            for (jindex=0; jindex<temp_size_c; jindex++) //Loop through rows of admittance matrices
            {
               for (kindex=0; kindex<temp_size_c; kindex++) //Loop through columns of admittance matrices
               {
                  //Indices counted out from Self admittance above.  needs doubling due to complex separation
                  if ((imag(Temp_Ad_A[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //From imags
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex*2;
                     Y_offdiag_PQ[indexer].Y_value = -(imag(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex + temp_size;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex*2
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].Y_value = (imag(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;
                  }

                  if ((imag(Temp_Ad_B[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //To imags
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex*2;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(imag(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex*2
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex + temp_size;
                     Y_offdiag_PQ[indexer].Y_value = imag(Temp_Ad_B[jindex][kindex]);
                     indexer += 1;
                  }

                  if ((real(Temp_Ad_A[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) &&
                        (bus[tempb].type != BusType::PV)) //From reals
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex + temp_size;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex*2;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex*2
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;
                  }

                  if ((real(Temp_Ad_B[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //To reals
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex*2
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex*2;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex + temp_size;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;
                  }
               } //column end
            } //row end
         } //end full ABD for to AC

         if ((!Full_Mat_A) && (!Full_Mat_B)) //Neither is a full ABC, or we aren't doing AC, so we don't care
         {
            for (jindex=0; jindex<temp_size_c; jindex++) //Loop through rows of admittance matrices
            {
               for (kindex=0; kindex<temp_size_c; kindex++) //Loop through columns of admittance matrices
               {

                  //Indices counted out from Self admittance above.  needs doubling due to complex separation
                  if ((imag(Temp_Ad_A[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //From imags
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(imag(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex + temp_size;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].Y_value = (imag(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;
                  }

                  if ((imag(Temp_Ad_B[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //To imags
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(imag(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex + temp_size;
                     Y_offdiag_PQ[indexer].Y_value = imag(Temp_Ad_B[jindex][kindex]);
                     indexer += 1;
                  }

                  if ((real(Temp_Ad_A[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //From reals
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex + temp_size;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempa].Matrix_Loc + temp_index + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + kindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_A[jindex][kindex]));
                     indexer += 1;
                  }

                  if ((real(Temp_Ad_B[jindex][kindex]) != 0) && (bus[tempa].type != BusType::PV) && (bus[tempb].type != BusType::PV))
                     //To reals
                  {
                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex
                        + temp_size_b;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;

                     Y_offdiag_PQ[indexer].row_ind = 2*bus[tempb].Matrix_Loc + temp_index_b + jindex;
                     Y_offdiag_PQ[indexer].col_ind = 2*bus[tempa].Matrix_Loc + temp_index + kindex + temp_size;
                     Y_offdiag_PQ[indexer].Y_value = -(real(Temp_Ad_B[jindex][kindex]));
                     indexer += 1;
                  }
               } //column end
            } //row end
         } //end not full ABC with AC on either side case
      } //end branch for

      // Build the fixed part of the diagonal PQ bus elements of 6n*6n Y_NR matrix.
      // This part will not be updated at each iteration.
      size_diag_fixed = 0;
      for (jindexer=0; jindexer<bus_count;jindexer++)
      {
         for (jindex=0; jindex<3; jindex++)
         {
            for (kindex=0; kindex<3; kindex++)
            {
               if (real(BA_diag[jindexer].Y[jindex][kindex]) != 0 && bus[jindexer].type != BusType::PV && jindex!=kindex)
                  size_diag_fixed += 1;
               if (imag(BA_diag[jindexer].Y[jindex][kindex]) != 0 && bus[jindexer].type != BusType::PV && jindex!=kindex)
                  size_diag_fixed += 1;
               else {}
            }
         }
      }
      if (Y_diag_fixed == NULL)
      {
         Y_diag_fixed = (Y_NR *)malloc((size_diag_fixed*2) *sizeof(Y_NR));
         // Store the row,column and value of the fixed part of the diagonal PQ bus elements of 6n*6n
         // Y_NR matrix.

         //Make sure it worked
         if (Y_diag_fixed == NULL)
            error("NR: Failed to allocate memory for one of the necessary matrices");

         //Update the max size
         max_size_diag_fixed = size_diag_fixed;
      }
      else if (size_diag_fixed > max_size_diag_fixed) //Something changed and we are bigger!!
      {
         //Destroy us!
         free(Y_diag_fixed);

         //Rebuild us, we have the technology
         Y_diag_fixed = (Y_NR *)malloc((size_diag_fixed*2) *sizeof(Y_NR));

         //Make sure it worked
         if (Y_diag_fixed == NULL)
            error("NR: Failed to allocate memory for one of the necessary matrices");

         //Store the new size
         max_size_diag_fixed = size_diag_fixed;

         //Flag for a reallocation
         NR_realloc_needed = true;
      }

      indexer = 0;
      for (jindexer=0; jindexer<bus_count;jindexer++) //Parse through bus list
      {
         for (jindex=0; jindex<BA_diag[jindexer].size; jindex++)
         {
            for (kindex=0; kindex<BA_diag[jindexer].size; kindex++)
            {
               if (imag(BA_diag[jindexer].Y[jindex][kindex]) != 0 && bus[jindexer].type != BusType::PV && jindex!=kindex)
               {
                  Y_diag_fixed[indexer].row_ind = 2*BA_diag[jindexer].row_ind + jindex;
                  Y_diag_fixed[indexer].col_ind = 2*BA_diag[jindexer].col_ind + kindex;
                  Y_diag_fixed[indexer].Y_value = imag(BA_diag[jindexer].Y[jindex][kindex]);
                  indexer += 1;

                  Y_diag_fixed[indexer].row_ind = 2*BA_diag[jindexer].row_ind + jindex +BA_diag[jindexer].size;
                  Y_diag_fixed[indexer].col_ind = 2*BA_diag[jindexer].col_ind + kindex +BA_diag[jindexer].size;
                  Y_diag_fixed[indexer].Y_value = -imag(BA_diag[jindexer].Y[jindex][kindex]);
                  indexer += 1;
               }

               if (real(BA_diag[jindexer].Y[jindex][kindex]) != 0 && bus[jindexer].type != BusType::PV && jindex!=kindex)
               {
                  Y_diag_fixed[indexer].row_ind = 2*BA_diag[jindexer].row_ind + jindex;
                  Y_diag_fixed[indexer].col_ind = 2*BA_diag[jindexer].col_ind + kindex +BA_diag[jindexer].size;
                  Y_diag_fixed[indexer].Y_value = real(BA_diag[jindexer].Y[jindex][kindex]);
                  indexer += 1;

                  Y_diag_fixed[indexer].row_ind = 2*BA_diag[jindexer].row_ind + jindex +BA_diag[jindexer].size;
                  Y_diag_fixed[indexer].col_ind = 2*BA_diag[jindexer].col_ind + kindex;
                  Y_diag_fixed[indexer].Y_value = real(BA_diag[jindexer].Y[jindex][kindex]);
                  indexer += 1;
               }
            }
         }
      } //End bus parse for fixed diagonal
   }

   /** Newton-Raphson solver
    *  Solves a power flow problem using the Newton-Raphson method
    *  @return n=0 on failure to complete a single iteration, n>0 to indicate success after n interations, or
    *  n<0 to indicate failure after n iterations. */
   int solver_nr(unsigned int bus_count, BUSDATA *bus, unsigned int branch_count, BRANCHDATA *branch,
         bool *bad_computations)
   {
      // TODO: The following static variables, added by Dan, are temporary replacements for Gridlab-D globals.
      static const int NR_iteration_limit = 500; // Max NR iterations.
      static const int NR_superLU_procs = 1; // Number of processors to request.
      static double * deltaI_NR;
      static bool NR_admit_change = true;

      static bool newiter;

      static unsigned int total_variables; // Total number of phases to be calculating (size of matrices).
      static unsigned int max_total_variables;

      static unsigned int max_size_diag_update;

      static Bus_admit *BA_diag;
         // Store the diagonal elements of the bus admittance matrix. The off_diag elements of bus admittance matrix
         // are equal to negative value of branch admittance.
      static unsigned int size_offdiag_PQ;
      static Y_NR *Y_offdiag_PQ;
         // Store the row,column and value of off_diagonal elements of 6n*6n Y_NR matrix. No PV bus is included.
      static unsigned int size_diag_fixed;
      static Y_NR *Y_diag_fixed;
         // Y_diag_fixed store the row,column and value of fixed diagonal elements of 6n*6n Y_NR matrix. No PV bus is
         // included.
      static Y_NR *Y_diag_update;
         // Y_diag_update store the row,column and value of updated diagonal elements of 6n*6n Y_NR matrix at each
         // iteration. No PV bus is included.
      static Y_NR *Y_Amatrix;
         // Y_Amatrix store all the elements of Amatrix in equation AX=B;
      static Y_NR *Y_Work_Amatrix;

      // Variables used to determine realloaction state
      static unsigned int prev_m;
      // Track size of matrix put into superLU form - may not need a realloc, but needs to be updated.
      static bool NR_realloc_needed;

      // Generic solver variables
      static NR_SOLVER_VARS matrices_LU;

      // SuperLU variables
      static int *perm_c, *perm_r;
      static SuperMatrix A_LU,B_LU;
      // Internal iteration counter - just NR limits
      int Iteration;

      // A matrix size variable
      unsigned int size_Amatrix;

      // Voltage mismatch tracking variable
      double Maxmismatch;

      // Temporary calculation variables
      double tempIcalcReal, tempIcalcImag;
      double tempPbus; // Store temporary value of active power load at each bus.
      double tempQbus; // Store the temporary value of reactive power load at each bus

      // Miscellaneous index variable
      unsigned int indexer, tempa, tempb, jindexer, kindexer;
      char jindex, kindex;
      char temp_index, temp_index_b;
      unsigned int temp_index_c;

      // Temporary load calculation variables
      Complex undeltacurr[3];          // Current to ground of A, B, C.
      Complex delta_current[3];        // dI = (S_const / dV)* + Y_const * dV;
      Complex voltageDel[3];           // Voltage diff between AB, AC, CA.

      // DV checking array
      Complex DVConvCheck[3];
      double CurrConvVal;

      // Miscellaneous working variable
      double work_vals_double_0, work_vals_double_1,work_vals_double_2, work_vals_double_3;
      char work_vals_char_0;

      // SuperLU variables
      SuperMatrix L_LU,U_LU;
      NCformat *Astore;
      DNformat *Bstore;
      int nnz, info;
      unsigned int m,n;
      double *sol_LU;

#ifndef MT
      superlu_options_t options;
      // Additional variables for sequential superLU
      SuperLUStat_t stat;
#endif

      // Ensure bad computations flag is set first
      *bad_computations = false;

      if (NR_admit_change) //If an admittance update was detected, fix it
      {
         buildAdmit(bus_count, bus, branch_count, branch, max_total_variables, total_variables, 
                    BA_diag, size_offdiag_PQ, Y_offdiag_PQ, size_diag_fixed, Y_diag_fixed, NR_realloc_needed);
         NR_admit_change = false;
      }

      int phase_mask[] = {0x04, 0x02, 0x01};    // A, B, C.
      int delta_mask[] = {0x06, 0x03, 0x05};   // AB, BC, CA.
      int pd_mat[3][3] =
      {  
         1,   -1,    0,
         0,    1,   -1,
         -1,    0,    1
      }; // pd_mat[idelta][iphase]
      int phase_map[3][2] =
      {
         1,   2,
         2,   3,
         3,   1
      }; // phase_map[idelta][{phase_from, phase_to}]

      // Calculate the system load - this is the specified power of the system
      for (Iteration=0; Iteration<NR_iteration_limit; Iteration++)
      {
         // System load at each bus is represented by second order polynomial equations
         for (indexer=0; indexer<bus_count; indexer++)
         {
            if ((bus[indexer].phases & 0x08) == 0x08) // Delta connected node
            {
               for (int idelta = 0; idelta < 3; ++idelta)
               {
                  int mask = delta_mask[idelta];
                  int ifrom = phase_map[idelta][1];
                  int ito = phase_map[idelta][2];
                  // Delta components - populate according to what is there
                  if ((bus[indexer].phases & mask) == mask) // Check for AB
                  {
                     // Voltage calculations
                     voltageDel[idelta] = bus[indexer].V[ifrom] - bus[indexer].V[ito];

                     // Power - convert to a current (uses less iterations this way)
                     delta_current[idelta] =
                        (voltageDel[idelta] == 0.0) ? 0 : conj(bus[indexer].S[idelta]/voltageDel[idelta]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[idelta] += voltageDel[idelta] * (bus[indexer].Y[idelta]);
                  }
                  else
                  {
                     // Zero values - they shouldn't be used anyhow
                     voltageDel[idelta] = 0.0;
                     delta_current[idelta] = 0.0;
                  }
               }

               // Convert delta-current into a phase current, where appropriate - reuse temp variable
               // Everything will be accumulated into the "current" field for ease (including differents)
               for (int iphase = 0; iphase < 3; ++iphase)
               {
                  undeltacurr[iphase] = czero;
                  int mask = phase_mask[iphase];
                  if ((bus[indexer].phases & mask) == mask) // Have phase mask present.
                  {
                     for (int idelta = 0; idelta < 3; ++idelta)
                     {
                        undeltacurr[iphase] += pd_mat[idelta][iphase] * (bus[indexer].I[idelta]+delta_current[idelta]);
                     }
                  }
               }

               // Provide updates to relevant phases
               // only compute and store phases that exist (make top heavy)
               temp_index = -1;
               temp_index_b = -1;

               for (jindex=0; jindex<BA_diag[indexer].size; jindex++)
               {
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index=0;
                           temp_index_b=2;
                           break;
                        }
                     case 0x02: // B
                        {
                           temp_index=0;
                           temp_index_b=1;
                           break;
                        }
                     case 0x03: // BC
                        {
                           if (jindex==0) // B
                           {
                              temp_index=0;
                              temp_index_b=1;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x04: // A
                        {
                           temp_index=0;
                           temp_index_b=0;
                           break;
                        }
                     case 0x05: // AC
                        {
                           if (jindex==0) // A
                           {
                              temp_index=0;
                              temp_index_b=0;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x06: // AB
                     case 0x07: // ABC
                        {
                           temp_index=jindex;
                           temp_index_b=jindex;
                           break;
                        }
                     default:
                        break;
                  }
                  // end case

                  if ((temp_index==-1) || (temp_index_b==-1))
                  {
                     error("NR: A scheduled power update element failed.");
                     // Defined below
                  }

                  // Real power calculations
                  tempPbus = real(undeltacurr[temp_index_b]) * real(bus[indexer].V[temp_index_b])
                     + imag(undeltacurr[temp_index_b]) * imag(bus[indexer].V[temp_index_b]);
                  // Real power portion of Constant current component multiply the magnitude of bus voltage
                  bus[indexer].PL[temp_index] = tempPbus;
                  // Real power portion - all is current based

                  // Reactive load calculations
                  tempQbus = real(undeltacurr[temp_index_b]) * imag(bus[indexer].V[temp_index_b])
                     - imag(undeltacurr[temp_index_b]) * real(bus[indexer].V[temp_index_b]);
                  // Reactive power portion of Constant current component multiply the magnitude of bus voltage
                  bus[indexer].QL[temp_index] = tempQbus;
                  // Reactive power portion - all is current based

               } // End phase traversion
            } // end delta connected
            else // Wye-connected node
            {
               // For Wye-connected, only compute and store phases that exist (make top heavy)
               temp_index = -1;
               temp_index_b = -1;

               undeltacurr[0] = undeltacurr[1] = undeltacurr[2] = 0.0;

               for (jindex=0; jindex<BA_diag[indexer].size; jindex++)
               {
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index=0;
                           temp_index_b=2;
                           break;
                        }
                     case 0x02: // B
                        {
                           temp_index=0;
                           temp_index_b=1;
                           break;
                        }
                     case 0x03: // BC
                        {
                           if (jindex==0) // B
                           {
                              temp_index=0;
                              temp_index_b=1;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x04: // A
                        {
                           temp_index=0;
                           temp_index_b=0;
                           break;
                        }
                     case 0x05: // AC
                        {
                           if (jindex==0) // A
                           {
                              temp_index=0;
                              temp_index_b=0;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x06: // AB
                     case 0x07: // ABC
                        {
                           temp_index=jindex;
                           temp_index_b=jindex;
                           break;
                        }
                     default:
                        break;
                  }
                  // end case

                  if ((temp_index==-1) || (temp_index_b==-1))
                  {
                     error("NR: A scheduled power update element failed.");
                     /*  TROUBLESHOOT
                         While attempting to calculate the scheduled portions of the
                         attached loads, an update failed to process correctly.
                         Submit you code and a bug report using the trac website.
                         */
                  }

                  // Perform the power calculation
                  tempPbus = real(bus[indexer].S[temp_index_b]);
                  // Real power portion of constant power portion
                  tempPbus += real(bus[indexer].I[temp_index_b]) * real(bus[indexer].V[temp_index_b])
                     + imag(bus[indexer].I[temp_index_b]) * imag(bus[indexer].V[temp_index_b]);
                  // Real power portion of Constant current component multiply the magnitude of bus voltage
                  tempPbus += real(undeltacurr[temp_index_b]) * real(bus[indexer].V[temp_index_b])
                     + imag(undeltacurr[temp_index_b]) * imag(bus[indexer].V[temp_index_b]);
                  // Real power portion of Constant current from "different" children
                  tempPbus += real(bus[indexer].Y[temp_index_b]) * real(bus[indexer].V[temp_index_b]) *
                     real(bus[indexer].V[temp_index_b]) + real(bus[indexer].Y[temp_index_b]) *
                     imag(bus[indexer].V[temp_index_b]) * imag(bus[indexer].V[temp_index_b]);
                  // Real power portion of Constant impedance component multiply the square of the magnitude of bus
                  // voltage
                  bus[indexer].PL[temp_index] = tempPbus;
                  // Real power portion


                  tempQbus = imag(bus[indexer].S[temp_index_b]);
                  // Reactive power portion of constant power portion
                  tempQbus += real(bus[indexer].I[temp_index_b]) * imag(bus[indexer].V[temp_index_b])
                     - imag(bus[indexer].I[temp_index_b]) * real(bus[indexer].V[temp_index_b]);
                  // Reactive power portion of Constant current component multiply the magnitude of bus voltage
                  tempQbus += real(undeltacurr[temp_index_b]) * imag(bus[indexer].V[temp_index_b])
                     - imag(undeltacurr[temp_index_b]) * real(bus[indexer].V[temp_index_b]);
                  // Reactive power portion of Constant current from "different" children
                  tempQbus += -imag(bus[indexer].Y[temp_index_b]) * imag(bus[indexer].V[temp_index_b]) *
                     imag(bus[indexer].V[temp_index_b]) - imag(bus[indexer].Y[temp_index_b]) *
                     real(bus[indexer].V[temp_index_b]) * real(bus[indexer].V[temp_index_b]);
                  // Reactive power portion of Constant impedance component multiply the square of the magnitude of
                  // bus voltage
                  bus[indexer].QL[temp_index] = tempQbus; // Reactive power portion
               } // end phase traversion
            } // end wye-connected
         } // end bus traversion for power update

         // Calculate the mismatch of three phase current injection at each bus (deltaI),
         // and store the deltaI in terms of real and reactive value in array deltaI_NR
         if (deltaI_NR==NULL)
         {
            deltaI_NR = (double *)malloc((2*total_variables) *sizeof(double));
            // left_hand side of equation (11)

            // Make sure it worked
            if (deltaI_NR == NULL)
               error("NR: Failed to allocate memory for one of the necessary matrices");

            // Update the max size
            max_total_variables = total_variables;
         }
         else if (NR_realloc_needed) // Bigger sized (this was checked above)
         {
            // Decimate the existing value
            free(deltaI_NR);

            // Reallocate it...bigger...faster...stronger!
            deltaI_NR = (double *)malloc((2*total_variables) *sizeof(double));

            // Make sure it worked
            if (deltaI_NR == NULL)
               error("NR: Failed to allocate memory for one of the necessary matrices");

            // Store the updated value
            max_total_variables = total_variables;
         }

         // Compute the calculated loads (not specified) at each bus
         for (indexer=0; indexer<bus_count; indexer++) // for specific bus k
         {
            for (jindex=0; jindex<BA_diag[indexer].size; jindex++) // rows - for specific phase that exists
            {
               tempIcalcReal = tempIcalcImag = 0;

               tempPbus =  - bus[indexer].PL[jindex];
               // @@@ PG and QG is assumed to be zero here @@@ - this may change later (PV busses)
               tempQbus =  - bus[indexer].QL[jindex];

               for (kindex=0; kindex<BA_diag[indexer].size; kindex++) // cols - Still only for specified phases
               {
                  // Determine our indices, based on phase information
                  temp_index = -1;
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index=2;
                           break;
                        }
                        // end 0x01
                     case 0x02: // B
                        {
                           temp_index=1;
                           break;
                        }
                        // end 0x02
                     case 0x03: // BC
                        {
                           if (kindex==0) // B
                              temp_index=1;
                           else // C
                              temp_index=2;
                           break;
                        }
                        // end 0x03
                     case 0x04: // A
                        {
                           temp_index=0;
                           break;
                        }
                        // end 0x04
                     case 0x05: // AC
                        {
                           if (kindex==0) // A
                              temp_index=0;
                           else // C
                              temp_index=2;
                           break;
                        }
                        // end 0x05
                     case 0x06: // AB
                        {
                           if (kindex==0) // A
                              temp_index=0;
                           else // B
                              temp_index=1;
                           break;
                        }
                        // end 0x06
                     case 0x07: // ABC
                        {
                           temp_index = kindex;
                           // Will loop all 3
                           break;
                        }
                        // end 0x07
                  } // End switch/case

                  if (temp_index==-1) // Error check
                  {
                     error("NR: A voltage index failed to be found.");
                     /*  TROUBLESHOOT
                         While attempting to compute the calculated power current, a voltage index failed to be
                         resolved.  Please submit your code and a bug report via the trac website.
                         */
                  }

                  // Diagonal contributions
                  tempIcalcReal += real(BA_diag[indexer].Y[jindex][kindex]) * real(bus[indexer].V[temp_index]) -
                     imag(BA_diag[indexer].Y[jindex][kindex]) * imag(bus[indexer].V[temp_index]);
                  // equation (7), the diag elements of bus admittance matrix
                  tempIcalcImag += real(BA_diag[indexer].Y[jindex][kindex]) * imag(bus[indexer].V[temp_index]) +
                     imag(BA_diag[indexer].Y[jindex][kindex]) * real(bus[indexer].V[temp_index]);
                  // equation (8), the diag elements of bus admittance matrix


                  // Off diagonal contributions
                  // Need another variable to handle the rows
                  temp_index_b = -1;
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index_b=2;
                           break;
                        }
                        // end 0x01
                     case 0x02: // B
                        {
                           temp_index_b=1;
                           break;
                        }
                        // end 0x02
                     case 0x03: // BC
                        {
                           if (jindex==0) // B
                              temp_index_b=1;
                           else // C
                              temp_index_b=2;
                           break;
                        }
                        // end 0x03
                     case 0x04: // A
                        {
                           temp_index_b=0;
                           break;
                        }
                        // end 0x04
                     case 0x05: // AC
                        {
                           if (jindex==0) // A
                              temp_index_b=0;
                           else // C
                              temp_index_b=2;
                           break;
                        }
                        // end 0x05
                     case 0x06: // AB
                        {
                           if (jindex==0) // A
                              temp_index_b=0;
                           else // B
                              temp_index_b=1;
                           break;
                        }
                        // end 0x06
                     case 0x07: // ABC
                        {
                           temp_index_b = jindex;
                           // Will loop all 3
                           break;
                        }
                        // end 0x07
                  } // End switch/case

                  if (temp_index_b==-1) // Error check
                  {
                     error("NR: A voltage index failed to be found.");
                  }

                  for (kindexer=0; kindexer<(bus[indexer].Link_Table.size()); kindexer++)
                     // Parse through the branch list
                  {
                     // Apply proper index to jindexer (easier to implement this way)
                     jindexer=bus[indexer].Link_Table[kindexer];

                     if (branch[jindexer].from == indexer)
                     {
                        work_vals_char_0 = temp_index_b*3+temp_index;
                        work_vals_double_0 = real(-branch[jindexer].Yfrom[work_vals_char_0]);
                        work_vals_double_1 = imag(-branch[jindexer].Yfrom[work_vals_char_0]);
                        work_vals_double_2 = real(bus[branch[jindexer].to].V[temp_index]);
                        work_vals_double_3 = imag(bus[branch[jindexer].to].V[temp_index]);

                        tempIcalcReal += work_vals_double_0 * work_vals_double_2 - work_vals_double_1 *
                           work_vals_double_3;
                        // equation (7), the off_diag elements of bus admittance matrix are equal to negative
                        // value of branch admittance
                        tempIcalcImag += work_vals_double_0 * work_vals_double_3 + work_vals_double_1 *
                           work_vals_double_2;
                        // equation (8), the off_diag elements of bus admittance matrix are equal to negative
                        // value of branch admittance

                        // end standard line

                     }
                     if  (branch[jindexer].to == indexer)
                     {
                        work_vals_char_0 = temp_index_b*3+temp_index;
                        work_vals_double_0 = real(-branch[jindexer].Yto[work_vals_char_0]);
                        work_vals_double_1 = imag(-branch[jindexer].Yto[work_vals_char_0]);
                        work_vals_double_2 = real(bus[branch[jindexer].from].V[temp_index]);
                        work_vals_double_3 = imag(bus[branch[jindexer].from].V[temp_index]);

                        tempIcalcReal += work_vals_double_0 * work_vals_double_2 - work_vals_double_1 *
                           work_vals_double_3;
                        // equation (7), the off_diag elements of bus admittance matrix are equal to negative value
                        // of branch admittance
                        tempIcalcImag += work_vals_double_0 * work_vals_double_3 + work_vals_double_1 *
                           work_vals_double_2;
                        // equation (8), the off_diag elements of bus admittance matrix are equal to negative value
                        // of branch admittance

                     }
                     else;

                  }
               }
               // end intermediate current for each phase column
               work_vals_double_0 = abs(bus[indexer].V[temp_index_b])*abs(bus[indexer].V[temp_index_b]);

               if (work_vals_double_0!=0)
                  // Only normal one (not square), but a zero is still a zero even after that
               {
                  work_vals_double_1 = real(bus[indexer].V[temp_index_b]);
                  work_vals_double_2 = imag(bus[indexer].V[temp_index_b]);
                  deltaI_NR[2*bus[indexer].Matrix_Loc+ BA_diag[indexer].size + jindex] = (tempPbus *
                        work_vals_double_1 + tempQbus * work_vals_double_2)/ (work_vals_double_0) - tempIcalcReal ;
                  // equation(7), Real part of deltaI, left hand side of equation (11)
                  deltaI_NR[2*bus[indexer].Matrix_Loc + jindex] = (tempPbus * work_vals_double_2 - tempQbus *
                        work_vals_double_1)/ (work_vals_double_0) - tempIcalcImag;
                  // Imaginary part of deltaI, left hand side of equation (11)
               }
               else
               {
                  deltaI_NR[2*bus[indexer].Matrix_Loc+BA_diag[indexer].size + jindex] = 0.0;
                  deltaI_NR[2*bus[indexer].Matrix_Loc + jindex] = 0.0;
               }
            } // End delta_I for each phase row
         } // End delta_I for each bus

         // Calculate the elements of a,b,c,d in equations(14),(15),(16),(17). These elements are used to update the
         // Jacobian matrix.
         for (indexer=0; indexer<bus_count; indexer++)
         {
            if ((bus[indexer].phases & 0x08) == 0x08) // Delta connected node
            {
               // Delta components - populate according to what is there
               if ((bus[indexer].phases & 0x06) == 0x06) // Check for AB
               {
                  // Voltage calculations
                  voltageDel[0] = bus[indexer].V[0] - bus[indexer].V[1];

                  // Power - convert to a current (uses less iterations this way)
                  delta_current[0] = (voltageDel[0] == 0.0) ? 0 : conj(bus[indexer].S[0]/voltageDel[0]);

                  // Convert delta connected load to appropriate Wye
                  delta_current[0] += voltageDel[0] * (bus[indexer].Y[0]);

               }
               else
               {
                  // Zero values - they shouldn't be used anyhow
                  voltageDel[0] = 0.0;
                  delta_current[0] = 0.0;
               }

               if ((bus[indexer].phases & 0x03) == 0x03) // Check for BC
               {
                  // Voltage calculations
                  voltageDel[1] = bus[indexer].V[1] - bus[indexer].V[2];

                  // Power - convert to a current (uses less iterations this way)
                  delta_current[1] = (voltageDel[1] == 0.0) ? 0 : conj(bus[indexer].S[1]/voltageDel[1]);

                  // Convert delta connected load to appropriate Wye
                  delta_current[1] += voltageDel[1] * (bus[indexer].Y[1]);

               }
               else
               {
                  // Zero unused
                  voltageDel[1] = 0.0;
                  delta_current[1] = 0.0;
               }

               if ((bus[indexer].phases & 0x05) == 0x05) // Check for CA
               {
                  // Voltage calculations
                  voltageDel[2] = bus[indexer].V[2] - bus[indexer].V[0];

                  // Power - convert to a current (uses less iterations this way)
                  delta_current[2] = (voltageDel[2] == 0.0) ? 0 : conj(bus[indexer].S[2]/voltageDel[2]);

                  // Convert delta connected load to appropriate Wye
                  delta_current[2] += voltageDel[2] * (bus[indexer].Y[2]);

               }
               else
               {
                  // Zero unused
                  voltageDel[2] = 0.0;
                  delta_current[2] = 0.0;
               }

               // Convert delta-current into a phase current, where appropriate - reuse temp variable
               // Everything will be accumulated into the "current" field for ease (including differents)
               if ((bus[indexer].phases & 0x04) == 0x04) // Has a phase A
               {
                  undeltacurr[0]=(bus[indexer].I[0]+delta_current[0])-(bus[indexer].I[2]+delta_current[2]);
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[0] = 0.0;
               }

               if ((bus[indexer].phases & 0x02) == 0x02) // Has a phase B
               {
                  undeltacurr[1]=(bus[indexer].I[1]+delta_current[1])-(bus[indexer].I[0]+delta_current[0]);
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[1] = 0.0;
               }


               if ((bus[indexer].phases & 0x01) == 0x01) // Has a phase C
               {
                  undeltacurr[2]=(bus[indexer].I[2]+delta_current[2])-(bus[indexer].I[1]+delta_current[1]);
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[2] = 0.0;
               }

               // Provide updates to relevant phases
               // only compute and store phases that exist (make top heavy)
               temp_index = -1;
               temp_index_b = -1;

               for (jindex=0; jindex<BA_diag[indexer].size; jindex++)
               {
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index=0;
                           temp_index_b=2;
                           break;
                        }
                     case 0x02: // B
                        {
                           temp_index=0;
                           temp_index_b=1;
                           break;
                        }
                     case 0x03: // BC
                        {
                           if (jindex==0) // B
                           {
                              temp_index=0;
                              temp_index_b=1;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x04: // A
                        {
                           temp_index=0;
                           temp_index_b=0;
                           break;
                        }
                     case 0x05: // AC
                        {
                           if (jindex==0) // A
                           {
                              temp_index=0;
                              temp_index_b=0;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x06: // AB
                     case 0x07: // ABC
                        {
                           temp_index=jindex;
                           temp_index_b=jindex;
                           break;
                        }
                     default:
                        break;
                  }
                  // end case

                  if ((temp_index==-1) || (temp_index_b==-1))
                  {
                     error("NR: A Jacobian update element failed.");
                     // Defined below
                  }

                  if (abs(bus[indexer].V[temp_index_b])!=0)
                  {
                     bus[indexer].Jacob_A[temp_index] = (real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*real(undeltacurr[temp_index_b]) +
                           imag(undeltacurr[temp_index_b]) *pow(imag(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // second part of equation(37) - no power term needed
                     bus[indexer].Jacob_B[temp_index] = -(real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*imag(undeltacurr[temp_index_b]) +
                           real(undeltacurr[temp_index_b]) *pow(real(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // second part of equation(38) - no power term needed
                     bus[indexer].Jacob_C[temp_index] =(real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*imag(undeltacurr[temp_index_b]) -
                           real(undeltacurr[temp_index_b]) *pow(imag(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // second part of equation(39) - no power term needed
                     bus[indexer].Jacob_D[temp_index] = (real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*real(undeltacurr[temp_index_b]) -
                           imag(undeltacurr[temp_index_b]) *pow(real(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // second part of equation(40) - no power term needed
                  }
                  else // Zero voltage = only impedance is valid (others get divided by VMag, so are IND)
                     // not entirely sure how this gets in here anyhow
                  {
                     // Small offset to avoid singularities (if impedance is zero too)
                     bus[indexer].Jacob_A[temp_index] = -1e-4;
                     bus[indexer].Jacob_B[temp_index] = -1e-4;
                     bus[indexer].Jacob_C[temp_index] = -1e-4;
                     bus[indexer].Jacob_D[temp_index] = -1e-4;
                  }
               } // End phase traversion
            } // end delta-connected load
            else // Wye-connected system/load
            {
               // For Wye-connected, only compute and store phases that exist (make top heavy)
               temp_index = -1;
               temp_index_b = -1;

               undeltacurr[0] = undeltacurr[1] = undeltacurr[2] = 0.0; // Zero it

               for (jindex=0; jindex<BA_diag[indexer].size; jindex++)
               {
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index=0;
                           temp_index_b=2;
                           break;
                        }
                     case 0x02: // B
                        {
                           temp_index=0;
                           temp_index_b=1;
                           break;
                        }
                     case 0x03: // BC
                        {
                           if (jindex==0) // B
                           {
                              temp_index=0;
                              temp_index_b=1;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x04: // A
                        {
                           temp_index=0;
                           temp_index_b=0;
                           break;
                        }
                     case 0x05: // AC
                        {
                           if (jindex==0) // A
                           {
                              temp_index=0;
                              temp_index_b=0;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x06: // AB
                     case 0x07: // ABC
                        {
                           temp_index=jindex;
                           temp_index_b=jindex;
                           break;
                        }
                     default:
                        break;
                  }
                  // end case

                  if ((temp_index==-1) || (temp_index_b==-1))
                  {
                     error("NR: A Jacobian update element failed.");
                     /*  TROUBLESHOOT
                         While attempting to calculate the "dynamic" portions of the
                         Jacobian matrix that encompass attached loads, an update failed to process correctly.
                         Submit you code and a bug report using the trac website.
                         */
                  }

                  if (abs(bus[indexer].V[temp_index_b])!=0)
                  {
                     bus[indexer].Jacob_A[temp_index] = (imag(bus[indexer].S[temp_index_b]) *
                           (pow(real(bus[indexer].V[temp_index_b]),2) - pow(imag(bus[indexer].V[temp_index_b]),2)) -
                           2*real(bus[indexer].V[temp_index_b])*imag(bus[indexer].V[temp_index_b])*
                           real(bus[indexer].S[temp_index_b]))/pow(abs(bus[indexer].V[temp_index_b]),4);
                     // first part of equation(37)
                     bus[indexer].Jacob_A[temp_index] += (real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*real(bus[indexer].I[temp_index_b]) +
                           imag(bus[indexer].I[temp_index_b]) *pow(imag(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3) + imag(bus[indexer].Y[temp_index_b]);
                     // second part of equation(37)
                     bus[indexer].Jacob_A[temp_index] += (real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*real(undeltacurr[temp_index_b]) +
                           imag(undeltacurr[temp_index_b]) *pow(imag(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // current part of equation (37) - Handles "different" children

                     bus[indexer].Jacob_B[temp_index] = (real(bus[indexer].S[temp_index_b]) *
                           (pow(real(bus[indexer].V[temp_index_b]),2) -
                            pow(imag(bus[indexer].V[temp_index_b]),2)) +
                           2*real(bus[indexer].V[temp_index_b])*imag(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].S[temp_index_b]))/pow(abs(bus[indexer].V[temp_index_b]),4);
                     // first part of equation(38)
                     bus[indexer].Jacob_B[temp_index] += -(real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*imag(bus[indexer].I[temp_index_b]) +
                           real(bus[indexer].I[temp_index_b]) *pow(real(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3) - real(bus[indexer].Y[temp_index_b]);
                     // second part of equation(38)
                     bus[indexer].Jacob_B[temp_index] += -(real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*imag(undeltacurr[temp_index_b]) +
                           real(undeltacurr[temp_index_b]) *pow(real(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // current part of equation(38) - Handles "different" children

                     bus[indexer].Jacob_C[temp_index] = (real(bus[indexer].S[temp_index_b]) *
                           (pow(imag(bus[indexer].V[temp_index_b]),2) - pow(real(bus[indexer].V[temp_index_b]),2)) -
                           2*real(bus[indexer].V[temp_index_b])*imag(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].S[temp_index_b]))/pow(abs(bus[indexer].V[temp_index_b]),4);
                     // first part of equation(39)
                     bus[indexer].Jacob_C[temp_index] +=(real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*imag(bus[indexer].I[temp_index_b]) -
                           real(bus[indexer].I[temp_index_b]) *pow(imag(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3) - real(bus[indexer].Y[temp_index_b]);
                     // second part of equation(39)
                     bus[indexer].Jacob_C[temp_index] +=(real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*imag(undeltacurr[temp_index_b]) -
                           real(undeltacurr[temp_index_b]) *pow(imag(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // Current part of equation(39) - Handles "different" children

                     bus[indexer].Jacob_D[temp_index] = (imag(bus[indexer].S[temp_index_b]) *
                           (pow(real(bus[indexer].V[temp_index_b]),2) - pow(imag(bus[indexer].V[temp_index_b]),2)) -
                           2*real(bus[indexer].V[temp_index_b])*imag(bus[indexer].V[temp_index_b])*
                           real(bus[indexer].S[temp_index_b]))/pow(abs(bus[indexer].V[temp_index_b]),4);
                     // first part of equation(40)
                     bus[indexer].Jacob_D[temp_index] += (real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*real(bus[indexer].I[temp_index_b]) -
                           imag(bus[indexer].I[temp_index_b]) *pow(real(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3) - imag(bus[indexer].Y[temp_index_b]);
                     // second part of equation(40)
                     bus[indexer].Jacob_D[temp_index] += (real(bus[indexer].V[temp_index_b])*
                           imag(bus[indexer].V[temp_index_b])*real(undeltacurr[temp_index_b]) -
                           imag(undeltacurr[temp_index_b]) *pow(real(bus[indexer].V[temp_index_b]),2))/
                        pow(abs(bus[indexer].V[temp_index_b]),3);
                     // Current part of equation(40) - Handles "different" children

                  }
                  else
                  {
                     bus[indexer].Jacob_A[temp_index]= imag(bus[indexer].Y[temp_index_b]) - 1e-4;
                     // Small offset to avoid singularity issues
                     bus[indexer].Jacob_B[temp_index]= -real(bus[indexer].Y[temp_index_b]) - 1e-4;
                     bus[indexer].Jacob_C[temp_index]= -real(bus[indexer].Y[temp_index_b]) - 1e-4;
                     bus[indexer].Jacob_D[temp_index]= -imag(bus[indexer].Y[temp_index_b]) - 1e-4;
                  }
               } // End phase traversion - Wye
            } // End wye-connected load
         }
         // end bus traversion for a,b,c, d value computation

         // Build the dynamic diagnal elements of 6n*6n Y matrix. All the elements in this part will be updated at
         // each iteration.
         unsigned int size_diag_update = 0;
         for (jindexer=0; jindexer<bus_count;jindexer++)
         {
            if  (bus[jindexer].type != BusType::PV) // PV bus ignored (for now?)
               size_diag_update += BA_diag[jindexer].size;
            else {}
         }

         if (Y_diag_update == NULL)
         {
            Y_diag_update = (Y_NR *)malloc((4*size_diag_update) *sizeof(Y_NR));
            // Y_diag_update store the row,column and value of the dynamic part of the diagonal PQ bus elements of
            // 6n*6n Y_NR matrix.

            // Make sure it worked
            if (Y_diag_update == NULL)
               error("NR: Failed to allocate memory for one of the necessary matrices");

            // Update maximum size
            max_size_diag_update = size_diag_update;
         }
         else if (size_diag_update > max_size_diag_update) // We've exceeded our limits
         {
            // Disappear the old one
            free(Y_diag_update);

            // Make a new one in its image
            Y_diag_update = (Y_NR *)malloc((4*size_diag_update) *sizeof(Y_NR));

            // Make sure it worked
            if (Y_diag_update == NULL)
               error("NR: Failed to allocate memory for one of the necessary matrices");

            // Update the size
            max_size_diag_update = size_diag_update;

            // Flag for a realloc
            NR_realloc_needed = true;
         }

         indexer = 0;
         // Rest positional counter

         for (jindexer=0; jindexer<bus_count; jindexer++) // Parse through bus list
         {
            if (bus[jindexer].type == BusType::SL) // Swing bus
            {
               for (jindex=0; jindex<BA_diag[jindexer].size; jindex++)
               {
                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind;
                  Y_diag_update[indexer].Y_value = 1e10;
                  // swing bus gets large admittance
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind + BA_diag[jindexer].size;
                  Y_diag_update[indexer].Y_value = 1e10;
                  // swing bus gets large admittance
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex + BA_diag[jindexer].size;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind - BA_diag[jindexer].size;
                  Y_diag_update[indexer].Y_value = 1e10;
                  // swing bus gets large admittance
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex + BA_diag[jindexer].size;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind;
                  Y_diag_update[indexer].Y_value = -1e10;
                  // swing bus gets large admittance
                  indexer += 1;
               } // End swing bus traversion
            } // End swing bus

            if (bus[jindexer].type != BusType::PV && bus[jindexer].type != BusType::SL) // No PV or swing (so must be PQ)
            {
               for (jindex=0; jindex<BA_diag[jindexer].size; jindex++)
               {
                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind;
                  Y_diag_update[indexer].Y_value = imag(BA_diag[jindexer].Y[jindex][jindex]) +
                     bus[jindexer].Jacob_A[jindex];
                  // Equation(14)
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind + BA_diag[jindexer].size;
                  Y_diag_update[indexer].Y_value = real(BA_diag[jindexer].Y[jindex][jindex]) +
                     bus[jindexer].Jacob_B[jindex];
                  // Equation(15)
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex + BA_diag[jindexer].size;
                  Y_diag_update[indexer].col_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].Y_value = real(BA_diag[jindexer].Y[jindex][jindex]) +
                     bus[jindexer].Jacob_C[jindex];
                  // Equation(16)
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex + BA_diag[jindexer].size;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind;
                  Y_diag_update[indexer].Y_value = -imag(BA_diag[jindexer].Y[jindex][jindex]) +
                     bus[jindexer].Jacob_D[jindex];
                  // Equation(17)
                  indexer += 1;
               }
               // end PQ phase traversion
            } // End PQ bus
         } // End bus parse list

         // Build the Amatrix, Amatrix includes all the elements of Y_offdiag_PQ, Y_diag_fixed and Y_diag_update.
         size_Amatrix = size_offdiag_PQ*2 + size_diag_fixed*2 + 4*size_diag_update;

         // Test to make sure it isn't an empty matrix - reliability induced 3-phase fault
         if (size_Amatrix==0)
         {
            warning("Empty powerflow connectivity matrix, your system is empty!");
            /*  TROUBLESHOOT
                Newton-Raphson has an empty admittance matrix that it is trying to solve.  Either the whole system
                faulted, or something is not properly defined.  Please try again.  If the problem persists, please
                submit your code and a bug report via the trac website.
                */

            *bad_computations = false;
            // Ensure output is flagged ok
            return 0;
            // Just return some arbitrary value - not technically bad
         }

         if (Y_Amatrix == NULL)
         {
            Y_Amatrix = (Y_NR *)malloc((size_Amatrix) *sizeof(Y_NR));
            // Amatrix includes all the elements of Y_offdiag_PQ, Y_diag_fixed and Y_diag_update.

            // Make sure it worked
            if (Y_Amatrix == NULL)
               error("NR: Failed to allocate memory for one of the necessary matrices");
         }
         else if (NR_realloc_needed) // If one of the above changed, we changed too
         {
            // Destroy the faulty version
            free(Y_Amatrix);

            // Create a new one that holds our new ampleness
            Y_Amatrix = (Y_NR *)malloc((size_Amatrix) *sizeof(Y_NR));

            // Make sure it worked
            if (Y_Amatrix == NULL)
               error("NR: Failed to allocate memory for one of the necessary matrices");
         }

         // integrate off diagonal components
         for (indexer=0; indexer<size_offdiag_PQ*2; indexer++)
         {
            Y_Amatrix[indexer].row_ind = Y_offdiag_PQ[indexer].row_ind;
            Y_Amatrix[indexer].col_ind = Y_offdiag_PQ[indexer].col_ind;
            Y_Amatrix[indexer].Y_value = Y_offdiag_PQ[indexer].Y_value;
         }

         // Integrate fixed portions of diagonal components
         for (indexer=size_offdiag_PQ*2; indexer< (size_offdiag_PQ*2 + size_diag_fixed*2); indexer++)
         {
            Y_Amatrix[indexer].row_ind = Y_diag_fixed[indexer - size_offdiag_PQ*2 ].row_ind;
            Y_Amatrix[indexer].col_ind = Y_diag_fixed[indexer - size_offdiag_PQ*2 ].col_ind;
            Y_Amatrix[indexer].Y_value = Y_diag_fixed[indexer - size_offdiag_PQ*2 ].Y_value;
         }

         // Integrate the variable portions of the diagonal components
         for (indexer=size_offdiag_PQ*2 + size_diag_fixed*2; indexer< size_Amatrix; indexer++)
         {
            Y_Amatrix[indexer].row_ind = Y_diag_update[indexer - size_offdiag_PQ*2 - size_diag_fixed*2].row_ind;
            Y_Amatrix[indexer].col_ind = Y_diag_update[indexer - size_offdiag_PQ*2 - size_diag_fixed*2].col_ind;
            Y_Amatrix[indexer].Y_value = Y_diag_update[indexer - size_offdiag_PQ*2 - size_diag_fixed*2].Y_value;
         }

         /* sorting integers */
         // Declare working array
         if (Y_Work_Amatrix == NULL)
         {
            Y_Work_Amatrix = (Y_NR *)malloc(size_Amatrix*sizeof(Y_NR));
            if (Y_Work_Amatrix==NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");
         }
         else if (NR_realloc_needed) // Y_Amatrix was likely resized, so we need it too since we's cousins
         {
            // Get rid of the old
            free(Y_Work_Amatrix);

            // And in with the new
            Y_Work_Amatrix = (Y_NR *)malloc(size_Amatrix*sizeof(Y_NR));
            if (Y_Work_Amatrix==NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");
         }

         merge_sort(Y_Amatrix, size_Amatrix, Y_Work_Amatrix);

#ifdef NR_MATRIX_OUT
         // Debugging code to export the sparse matrix values - useful for debugging issues, but needs preprocessor
         // declaration

         // Open a text file
         FILE *FPoutVal=fopen("matrixinfoout.txt","wt");

         // Print the values - printed as "row index, column index, value"
         // This particular output is after they have been column sorted for the algorithm
         for (jindexer=0; jindexer<size_Amatrix; jindexer++)
         {
            fprintf(FPoutVal,"%d,%d,%f\n",Y_Amatrix[jindexer].row_ind,Y_Amatrix[jindexer].col_ind,
                  Y_Amatrix[jindexer].Y_value);
         }

         // Close the file, we're done with it
         fclose(FPoutVal);
#endif

         // /* Initialize parameters. */
         m = 2*total_variables; n = 2*total_variables; nnz = size_Amatrix;

         if (matrices_LU.a_LU == NULL) // First run
         {
            /* Set aside space for the arrays. */
            matrices_LU.a_LU = (double *) malloc(nnz *sizeof(double));
            if (matrices_LU.a_LU==NULL)
            {
               error("NR: One of the SuperLU solver matrices failed to allocate");
               /*  TROUBLESHOOT
                   While attempting to allocate the memory for one of the SuperLU working matrices,
                   an error was encountered and it was not allocated.  Please try again.  If it fails
                   again, please submit your code and a bug report using the trac website.
                   */
            }

            matrices_LU.rows_LU = (int *) malloc(nnz *sizeof(int));
            if (matrices_LU.rows_LU == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            matrices_LU.cols_LU = (int *) malloc((n+1) *sizeof(int));
            if (matrices_LU.cols_LU == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            /* Create the right-hand side matrix B. */
            matrices_LU.rhs_LU = (double *) malloc(m *sizeof(double));
            if (matrices_LU.rhs_LU == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            // /* Set up the arrays for the permutations. */
            perm_r = (int *) malloc(m *sizeof(int));
            if (perm_r == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            perm_c = (int *) malloc(n *sizeof(int));
            if (perm_c == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            // Set up storage pointers - single element, but need to be malloced for some reason
            A_LU.Store = (void *)malloc(sizeof(NCformat));
            if (A_LU.Store == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            B_LU.Store = (void *)malloc(sizeof(DNformat));
            if (B_LU.Store == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            // Populate these structures - A_LU matrix
            A_LU.Stype = SLU_NC;
            A_LU.Dtype = SLU_D;
            A_LU.Mtype = SLU_GE;
            A_LU.nrow = n;
            A_LU.ncol = m;

            // Populate these structures - B_LU matrix
            B_LU.Stype = SLU_DN;
            B_LU.Dtype = SLU_D;
            B_LU.Mtype = SLU_GE;
            B_LU.nrow = m;
            B_LU.ncol = 1;

            // Update tracking variable
            prev_m = m;
         }
         else if (NR_realloc_needed) // Something changed, we'll just destroy everything and start over
         {
            // Get rid of all of them first
            free(matrices_LU.a_LU);
            free(matrices_LU.rows_LU);
            free(matrices_LU.cols_LU);
            free(matrices_LU.rhs_LU);

            // Free up superLU matrices
            free(perm_r);
            free(perm_c);

            /* Set aside space for the arrays. - Copied from above */
            matrices_LU.a_LU = (double *) malloc(nnz *sizeof(double));
            if (matrices_LU.a_LU==NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            matrices_LU.rows_LU = (int *) malloc(nnz *sizeof(int));
            if (matrices_LU.rows_LU == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            matrices_LU.cols_LU = (int *) malloc((n+1) *sizeof(int));
            if (matrices_LU.cols_LU == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            /* Create the right-hand side matrix B. */
            matrices_LU.rhs_LU = (double *) malloc(m *sizeof(double));
            if (matrices_LU.rhs_LU == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            // /* Set up the arrays for the permutations. */
            perm_r = (int *) malloc(m *sizeof(int));
            if (perm_r == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            perm_c = (int *) malloc(n *sizeof(int));
            if (perm_c == NULL)
               error("NR: One of the SuperLU solver matrices failed to allocate");

            // Update structures - A_LU matrix
            A_LU.Stype = SLU_NC;
            A_LU.Dtype = SLU_D;
            A_LU.Mtype = SLU_GE;
            A_LU.nrow = n;
            A_LU.ncol = m;

            // Update structures - B_LU matrix
            B_LU.Stype = SLU_DN;
            B_LU.Dtype = SLU_D;
            B_LU.Mtype = SLU_GE;
            B_LU.nrow = m;
            B_LU.ncol = 1;

            // Update tracking variable
            prev_m = m;
         }
         else if (prev_m != m) // Non-reallocing size change occurred
         {
            // Update relevant portions
            A_LU.nrow = n;
            A_LU.ncol = m;

            B_LU.nrow = m;

            // Update tracking variable
            prev_m = m;
         }

#ifndef MT
         /* superLU sequential options*/
         set_default_options ( &options );
#endif

         for (indexer=0; indexer<size_Amatrix; indexer++)
         {
            matrices_LU.rows_LU[indexer] = Y_Amatrix[indexer].row_ind ;
            // row pointers of non zero values
            matrices_LU.a_LU[indexer] = Y_Amatrix[indexer].Y_value;
         }
         matrices_LU.cols_LU[0] = 0;
         indexer = 0;
         temp_index_c = 0;
         for ( jindexer = 0; jindexer< (size_Amatrix-1); jindexer++)
         {
            indexer += 1;
            tempa = Y_Amatrix[jindexer].col_ind;
            tempb = Y_Amatrix[jindexer+1].col_ind;
            if (tempb > tempa)
            {
               temp_index_c += 1;
               matrices_LU.cols_LU[temp_index_c] = indexer;
            }
         }
         matrices_LU.cols_LU[n] = nnz ;
         // number of non-zeros;

         for (temp_index_c=0;temp_index_c<m;temp_index_c++)
         {
            matrices_LU.rhs_LU[temp_index_c] = deltaI_NR[temp_index_c];
         }

         //
         //* Create Matrix A in the format expected by Super LU.*/
         // Populate the matrix values (temporary value)
         Astore = (NCformat*)A_LU.Store;
         Astore->nnz = nnz;
         Astore->nzval = matrices_LU.a_LU;
         Astore->rowind = matrices_LU.rows_LU;
         Astore->colptr = matrices_LU.cols_LU;

         // Create right-hand side matrix B in format expected by Super LU
         // Populate the matrix (temporary values)
         Bstore = (DNformat*)B_LU.Store;
         Bstore->lda = m;
         Bstore->nzval = matrices_LU.rhs_LU;

#ifdef MT // superLU_MT commands

         // Populate perm_c
         get_perm_c(1, &A_LU, perm_c);

         // Solve the system
         pdgssv(NR_superLU_procs, &A_LU, perm_c, perm_r, &L_LU, &U_LU, &B_LU, &info);
#else // sequential superLU

         StatInit ( &stat );

         // solve the system
         dgssv(&options, &A_LU, perm_c, perm_r, &L_LU, &U_LU, &B_LU, &stat, &info);
#endif

         sol_LU = (double*) ((DNformat*) B_LU.Store)->nzval;

         // Update bus voltages - check convergence while we're here
         Maxmismatch = 0;

         temp_index = -1;
         temp_index_b = -1;
         newiter = false;
         // Reset iteration requester flag - defaults to not needing another

         for (indexer=0; indexer<bus_count; indexer++)
         {
            // Avoid swing bus updates
            if (bus[indexer].type != BusType::SL)
            {
               // Figure out the offset we need to be for each phase
               for (jindex=0; jindex<BA_diag[indexer].size; jindex++) // parse through the phases
               {
                  switch(bus[indexer].phases & 0x07) {
                     case 0x01: // C
                        {
                           temp_index=0;
                           temp_index_b=2;
                           break;
                        }
                     case 0x02: // B
                        {
                           temp_index=0;
                           temp_index_b=1;
                           break;
                        }
                     case 0x03: // BC
                        {
                           if (jindex==0) // B
                           {
                              temp_index=0;
                              temp_index_b=1;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }

                           break;
                        }
                     case 0x04: // A
                        {
                           temp_index=0;
                           temp_index_b=0;
                           break;
                        }
                     case 0x05: // AC
                        {
                           if (jindex==0) // A
                           {
                              temp_index=0;
                              temp_index_b=0;
                           }
                           else // C
                           {
                              temp_index=1;
                              temp_index_b=2;
                           }
                           break;
                        }
                     case 0x06: // AB
                     case 0x07: // ABC
                        {
                           temp_index = jindex;
                           temp_index_b = jindex;
                           break;
                        }
                  }
                  // end phase switch/case

                  if ((temp_index==-1) || (temp_index_b==-1))
                  {
                     error("NR: An error occurred indexing voltage updates");
                     /*  TROUBLESHOOT
                         While attempting to create the voltage update indices for the
                         Newton-Raphson solver, an error was encountered.  Please submit
                         your code and a bug report using the trac website.
                         */
                  }

                  DVConvCheck[jindex]=Complex(sol_LU[(2*bus[indexer].Matrix_Loc+temp_index)],
                        sol_LU[(2*bus[indexer].Matrix_Loc+BA_diag[indexer].size+temp_index)]);
                  bus[indexer].V[temp_index_b] += DVConvCheck[jindex];

                  // Pull off the magnitude (no sense calculating it twice)
                  CurrConvVal=abs(DVConvCheck[jindex]);
                  if (CurrConvVal > bus[indexer].max_volt_error) // Check for convergence
                     newiter=true;
                  // Flag that a new iteration must occur

                  if (CurrConvVal > Maxmismatch)
                     // See if the current differential is the largest found so far or not
                     Maxmismatch = CurrConvVal;
                  // It is, store it

               } // End For loop for phase traversion
            }
            // end if not swing
            else // So this must be the swing
            {
               temp_index +=2*BA_diag[indexer].size;
               // Increment us for what this bus would have been had it not been a swing
            }
         } // End bus traversion

         // Turn off reallocation flag no matter what
         NR_realloc_needed = false;

         /* De-allocate storage - superLU matrix types must be destroyed at every iteration, otherwise they balloon
          * fast (65 MB norma becomes 1.5 GB) */
#ifdef MT // superLU_MT commands
         Destroy_SuperNode_SCP(&L_LU);
         Destroy_CompCol_NCP(&U_LU);
#else // sequential superLU commands
         Destroy_SuperNode_Matrix( &L_LU );
         Destroy_CompCol_Matrix( &U_LU );
         StatFree ( &stat );
#endif

         // Break us out if we are done or are singular
         if (( newiter == false ) || (info!=0))
         {
            if (newiter == false)
            {
               message("Power flow calculation converges at Iteration %d \n",Iteration+1);
            }
            break;
         }
      } // End iteration loop

      // Check to see how we are ending
      if ((Iteration==NR_iteration_limit) && (newiter==true)) // Reached the limit
      {
         message("Max solver mismatch of failed solution %f\n",Maxmismatch);
         return -Iteration;
      }
      else if (info!=0) // failure of computations (singular matrix, etc.)
      {
         // For superLU - 2 = singular matrix it appears - positive values = process errors (singular, etc), negative
         // values = input argument/syntax error
         message("superLU failed out with return value %d",info);

         *bad_computations = true;
         // Flag our output as bad
         return 0;
         // Just return some arbitrary value
      }
      else // Must have converged
         return Iteration;
   }
} // namespace SmartGridToolbox
