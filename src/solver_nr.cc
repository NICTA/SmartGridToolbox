#include "solver_nr.h"

// Note: due to extensive indentation, line lengths are increased from 80 to 120 characters.

#define MT // this enables multithreaded SuperLU

// #define NR_MATRIX_OUT
// This directive enables a text file dump of the sparse-formatted admittance matrix - useful for debugging

#ifdef MT
#include <pdsp_defs.h> // superLU_MT
#else
#include <slu_ddefs.h> // Sequential superLU (other platforms)
#endif

namespace SmartGridToolbox
{
   static unsigned int size_offdiag_PQ;
   static unsigned int size_diag_fixed;
   static unsigned int total_variables; // Total number of phases to be calculating (size of matrices).
   static unsigned int max_size_offdiag_PQ, max_size_diag_fixed, max_total_variables, max_size_diag_update;
      // Variables used to determine realloaction state
   static unsigned int prev_m;
      // Track size of matrix put into superLU form - may not need a realloc, but needs to be updated.
   static bool NR_realloc_needed;
   static bool newiter;

   static Bus_admit *BA_diag;
      // Store the diagonal elements of the bus admittance matrix. The off_diag elements of bus admittance matrix 
      // are equal to negative value of branch admittance.

   static Y_NR *Y_offdiag_PQ;
      // Store the row,column and value of off_diagonal elements of 6n*6n Y_NR matrix. No PV bus is included.
   static Y_NR *Y_diag_fixed;
      // Y_diag_fixed store the row,column and value of fixed diagonal elements of 6n*6n Y_NR matrix. No PV bus is
      // included.
   static Y_NR *Y_diag_update;
      // Y_diag_update store the row,column and value of updated diagonal elements of 6n*6n Y_NR matrix at each
      // iteration. No PV bus is included.
   static Y_NR *Y_Amatrix;
      // Y_Amatrix store all the elements of Amatrix in equation AX=B;
   static Y_NR *Y_Work_Amatrix;

   // Generic solver variables
   static NR_SOLVER_VARS matrices_LU;

   // SuperLU variables
   static int *perm_c, *perm_r;
   static SuperMatrix A_LU,B_LU;

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
                  GL_THROW("NR: duplicate entry found in admittance matrix. Look for parallel lines!");
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

   /** Newton-Raphson solver
    *  Solves a power flow problem using the Newton-Raphson method
    *  @return n=0 on failure to complete a single iteration, n>0 to indicate success after n interations, or
    *  n<0 to indicate failure after n iterations. */
   int solver_nr(unsigned int bus_count, BUSDATA *bus, unsigned int branch_count, BRANCHDATA *branch,
                 bool *bad_computations)
   {
      // Internal iteration counter - just NR limits
      int64 Iteration;

      // A matrix size variable
      unsigned int size_Amatrix;

      // Voltage mismatch tracking variable
      double Maxmismatch;

      // Phase collapser variable
      unsigned char phase_worka, phase_workb, phase_workc, phase_workd, phase_worke;

      // Temporary calculation variables
      double tempIcalcReal, tempIcalcImag;
      double tempPbus;
      // Store temporary value of active power load at each bus.
      double tempQbus;
      // Store the temporary value of reactive power load at each bus

      // Miscellaneous index variable
      unsigned int indexer, tempa, tempb, jindexer, kindexer;
      char jindex, kindex;
      char temp_index, temp_index_b;
      unsigned int temp_index_c;

      // Working matrix for admittance collapsing/determinations
      complex tempY[3][3];

      // Miscellaneous flag variables
      bool Full_Mat_A, Full_Mat_B, proceed_flag;

      // Temporary size variable
      char temp_size, temp_size_b, temp_size_c;

      // Temporary admittance variables
      complex Temp_Ad_A[3][3];
      complex Temp_Ad_B[3][3];

      // Temporary load calculation variables
      complex undeltacurr[3], undeltaimped[3], undeltapower[3];
      complex delta_current[3], voltageDel[3];
      complex temp_current[3], temp_power[3], temp_store[3];

      // DV checking array
      complex DVConvCheck[3];
      double CurrConvVal;

      // Miscellaneous counter tracker
      unsigned int index_count = 0;

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

      // Calculate the system load - this is the specified power of the system
      for (Iteration=0; Iteration<NR_iteration_limit; Iteration++)
      {
         // System load at each bus is represented by second order polynomial equations
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
                  delta_current[0] = (voltageDel[0] == 0) ? 0 : ~(bus[indexer].S[0]/voltageDel[0]);

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
                  delta_current[1] = (voltageDel[1] == 0) ? 0 : ~(bus[indexer].S[1]/voltageDel[1]);

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
                  delta_current[2] = (voltageDel[2] == 0) ? 0 : ~(bus[indexer].S[2]/voltageDel[2]);

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

                  // Check for "different" children and apply them, as well
                  if ((bus[indexer].phases & 0x10) == 0x10) // We do, so they must be Wye-connected
                  {
                     // Power values
                     undeltacurr[0] += (bus[indexer].V[0] == 0) ? 0 : ~(bus[indexer].extra_var[0]/bus[indexer].V[0]);

                     // Shunt values
                     undeltacurr[0] += bus[indexer].extra_var[3]*bus[indexer].V[0];

                     // Current values
                     undeltacurr[0] += bus[indexer].extra_var[6];
                  }
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[0] = 0.0;
               }

               if ((bus[indexer].phases & 0x02) == 0x02) // Has a phase B
               {
                  undeltacurr[1]=(bus[indexer].I[1]+delta_current[1])-(bus[indexer].I[0]+delta_current[0]);

                  // Check for "different" children and apply them, as well
                  if ((bus[indexer].phases & 0x10) == 0x10) // We do, so they must be Wye-connected
                  {
                     // Power values
                     undeltacurr[1] += (bus[indexer].V[1] == 0) ? 0 : ~(bus[indexer].extra_var[1]/bus[indexer].V[1]);

                     // Shunt values
                     undeltacurr[1] += bus[indexer].extra_var[4]*bus[indexer].V[1];

                     // Current values
                     undeltacurr[1] += bus[indexer].extra_var[7];
                  }
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[1] = 0.0;
               }


               if ((bus[indexer].phases & 0x01) == 0x01) // Has a phase C
               {
                  undeltacurr[2]=(bus[indexer].I[2]+delta_current[2])-(bus[indexer].I[1]+delta_current[1]);

                  // Check for "different" children and apply them, as well
                  if ((bus[indexer].phases & 0x10) == 0x10) // We do, so they must be Wye-connected
                  {
                     // Power values
                     undeltacurr[2] += (bus[indexer].V[2] == 0) ? 0 : ~(bus[indexer].extra_var[2]/bus[indexer].V[2]);

                     // Shunt values
                     undeltacurr[2] += bus[indexer].extra_var[5]*bus[indexer].V[2];

                     // Current values
                     undeltacurr[2] += bus[indexer].extra_var[8];
                  }
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
                     GL_THROW("NR: A scheduled power update element failed.");
                     // Defined below
                  }

                  // Real power calculations
                  tempPbus = (undeltacurr[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Re() 
                     + (undeltacurr[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Im();
                  // Real power portion of Constant current component multiply the magnitude of bus voltage
                  bus[indexer].PL[temp_index] = tempPbus;
                  // Real power portion - all is current based

                  // Reactive load calculations
                  tempQbus = (undeltacurr[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Im() 
                     - (undeltacurr[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Re();
                  // Reactive power portion of Constant current component multiply the magnitude of bus voltage
                  bus[indexer].QL[temp_index] = tempQbus;
                  // Reactive power portion - all is current based

               } // End phase traversion
            } // end delta connected
            else if ((bus[indexer].phases & 0x80) == 0x80) // Split-phase connected node
            {
               // Convert it all back to current (easiest to handle)
               // Get V12 first
               voltageDel[0] = bus[indexer].V[0] + bus[indexer].V[1];

               // Start with the currents (just put them in)
               temp_current[0] = bus[indexer].I[0];
               temp_current[1] = bus[indexer].I[1];
               temp_current[2] = *bus[indexer].extra_var;
               // Current12 is not part of the standard current array

               // Now add in power contributions
               temp_current[0] += bus[indexer].V[0] == 0.0 ? 0.0 : ~(bus[indexer].S[0]/bus[indexer].V[0]);
               temp_current[1] += bus[indexer].V[1] == 0.0 ? 0.0 : ~(bus[indexer].S[1]/bus[indexer].V[1]);
               temp_current[2] += voltageDel[0] == 0.0 ? 0.0 : ~(bus[indexer].S[2]/voltageDel[0]);

               // Last, but not least, admittance/impedance contributions
               temp_current[0] += bus[indexer].Y[0]*bus[indexer].V[0];
               temp_current[1] += bus[indexer].Y[1]*bus[indexer].V[1];
               temp_current[2] += bus[indexer].Y[2]*voltageDel[0];

               // See if we are a house-connected node, if so, adjust and add in those values as well
               if ((bus[indexer].phases & 0x40) == 0x40)
               {
                  // Update phase adjustments
                  temp_store[0].SetPolar(1.0,bus[indexer].V[0].Arg());
                  // Pull phase of V1
                  temp_store[1].SetPolar(1.0,bus[indexer].V[1].Arg());
                  // Pull phase of V2
                  temp_store[2].SetPolar(1.0,voltageDel[0].Arg());
                  // Pull phase of V12

                  // Update these current contributions (use delta current variable, it isn't used in here anyways)
                  delta_current[0] = bus[indexer].house_var[0]/(~temp_store[0]);
                  // Just denominator conjugated to keep math right (rest was conjugated in house)
                  delta_current[1] = bus[indexer].house_var[1]/(~temp_store[1]);
                  delta_current[2] = bus[indexer].house_var[2]/(~temp_store[2]);

                  // Now add it into the current contributions
                  temp_current[0] += delta_current[0];
                  temp_current[1] += delta_current[1];
                  temp_current[2] += delta_current[2];
               } // End house-attached splitphase

               // Convert 'em to line currents
               temp_store[0] = temp_current[0] + temp_current[2];
               temp_store[1] = -temp_current[1] - temp_current[2];

               // Update the stored values
               bus[indexer].PL[0] = temp_store[0].Re();
               bus[indexer].QL[0] = temp_store[0].Im();

               bus[indexer].PL[1] = temp_store[1].Re();
               bus[indexer].QL[1] = temp_store[1].Im();
            } // end split-phase connected
            else // Wye-connected node
            {
               // For Wye-connected, only compute and store phases that exist (make top heavy)
               temp_index = -1;
               temp_index_b = -1;

               if ((bus[indexer].phases & 0x10) == 0x10)
                  // "Different" child load - in this case it must be delta - also must be three phase (just because
                  // that's how I forced it to be implemented)
               {
                  // Calculate all the deltas to wyes in advance (otherwise they'll get repeated)
                  // Delta voltages
                  voltageDel[0] = bus[indexer].V[0] - bus[indexer].V[1];
                  voltageDel[1] = bus[indexer].V[1] - bus[indexer].V[2];
                  voltageDel[2] = bus[indexer].V[2] - bus[indexer].V[0];

                  // Make sure phase combinations exist
                  if ((bus[indexer].phases & 0x06) == 0x06) // Has A-B
                  {
                     // Power - put into a current value (iterates less this way)
                     delta_current[0] = (voltageDel[0] == 0) ? 0 : ~(bus[indexer].extra_var[0]/voltageDel[0]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[0] += voltageDel[0] * (bus[indexer].extra_var[3]);
                  }
                  else
                  {
                     // Zero it, for good measure
                     delta_current[0] = 0.0;
                  }

                  // Check for BC
                  if ((bus[indexer].phases & 0x03) == 0x03) // Has B-C
                  {
                     // Power - put into a current value (iterates less this way)
                     delta_current[1] = (voltageDel[1] == 0) ? 0 : ~(bus[indexer].extra_var[1]/voltageDel[1]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[1] += voltageDel[1] * (bus[indexer].extra_var[4]);
                  }
                  else
                  {
                     // Zero it, for good measure
                     delta_current[1] = 0.0;
                  }

                  // Check for CA
                  if ((bus[indexer].phases & 0x05) == 0x05) // Has C-A
                  {
                     // Power - put into a current value (iterates less this way)
                     delta_current[2] = (voltageDel[2] == 0) ? 0 : ~(bus[indexer].extra_var[2]/voltageDel[2]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[2] += voltageDel[2] * (bus[indexer].extra_var[5]);
                  }
                  else
                  {
                     // Zero it, for good measure
                     delta_current[2] = 0.0;
                  }

                  // Convert delta-current into a phase current - reuse temp variable
                  undeltacurr[0]=(bus[indexer].extra_var[6]+delta_current[0])-(bus[indexer].extra_var[8]
                        +delta_current[2]);
                  undeltacurr[1]=(bus[indexer].extra_var[7]+delta_current[1])-(bus[indexer].extra_var[6]
                        +delta_current[0]);
                  undeltacurr[2]=(bus[indexer].extra_var[8]+delta_current[2])-(bus[indexer].extra_var[7]
                        +delta_current[1]);
               }
               else // zero the variable so we don't have excessive ifs
               {
                  undeltacurr[0] = undeltacurr[1] = undeltacurr[2] = 0.0;
                  // Zero it
               }

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
                     GL_THROW("NR: A scheduled power update element failed.");
                     /*  TROUBLESHOOT
                         While attempting to calculate the scheduled portions of the
                         attached loads, an update failed to process correctly.
                         Submit you code and a bug report using the trac website.
                         */
                  }

                  // Perform the power calculation
                  tempPbus = (bus[indexer].S[temp_index_b]).Re();
                  // Real power portion of constant power portion
                  tempPbus += (bus[indexer].I[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Re() 
                     + (bus[indexer].I[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Im();
                  // Real power portion of Constant current component multiply the magnitude of bus voltage
                  tempPbus += (undeltacurr[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Re() 
                     + (undeltacurr[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Im();
                  // Real power portion of Constant current from "different" children
                  tempPbus += (bus[indexer].Y[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Re() * 
                     (bus[indexer].V[temp_index_b]).Re() + (bus[indexer].Y[temp_index_b]).Re() * 
                     (bus[indexer].V[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Im();
                  // Real power portion of Constant impedance component multiply the square of the magnitude of bus 
                  // voltage
                  bus[indexer].PL[temp_index] = tempPbus;
                  // Real power portion


                  tempQbus = (bus[indexer].S[temp_index_b]).Im();
                  // Reactive power portion of constant power portion
                  tempQbus += (bus[indexer].I[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Im() 
                     - (bus[indexer].I[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Re();
                  // Reactive power portion of Constant current component multiply the magnitude of bus voltage
                  tempQbus += (undeltacurr[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Im() 
                     - (undeltacurr[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Re();
                  // Reactive power portion of Constant current from "different" children
                  tempQbus += -(bus[indexer].Y[temp_index_b]).Im() * (bus[indexer].V[temp_index_b]).Im() * 
                     (bus[indexer].V[temp_index_b]).Im() - (bus[indexer].Y[temp_index_b]).Im() * 
                     (bus[indexer].V[temp_index_b]).Re() * (bus[indexer].V[temp_index_b]).Re();
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
            deltaI_NR = (double *)gl_malloc((2*total_variables) *sizeof(double));
            // left_hand side of equation (11)

            // Make sure it worked
            if (deltaI_NR == NULL)
               GL_THROW("NR: Failed to allocate memory for one of the necessary matrices");

            // Update the max size
            max_total_variables = total_variables;
         }
         else if (NR_realloc_needed) // Bigger sized (this was checked above)
         {
            // Decimate the existing value
            gl_free(deltaI_NR);

            // Reallocate it...bigger...faster...stronger!
            deltaI_NR = (double *)gl_malloc((2*total_variables) *sizeof(double));

            // Make sure it worked
            if (deltaI_NR == NULL)
               GL_THROW("NR: Failed to allocate memory for one of the necessary matrices");

            // Store the updated value
            max_total_variables = total_variables;
         }

         // Compute the calculated loads (not specified) at each bus
         for (indexer=0; indexer<bus_count; indexer++) // for specific bus k
         {
            for (jindex=0; jindex<BA_diag[indexer].size; jindex++) // rows - for specific phase that exists
            {
               tempIcalcReal = tempIcalcImag = 0;

               if ((bus[indexer].phases & 0x80) == 0x80) // Split phase - triplex bus
               {
                  // Two states of triplex bus - To node of SPCT transformer needs to be different
                  // First different - Delta-I and diagonal contributions
                  if ((bus[indexer].phases & 0x20) == 0x20) // We're the To bus
                  {
                     // Pre-negated due to the nature of how it's calculated (V1 compared to I1)
                     tempPbus =  bus[indexer].PL[jindex];
                     // @@@ PG and QG is assumed to be zero here @@@ - this may change later (PV busses)
                     tempQbus =  bus[indexer].QL[jindex];
                  }
                  else // We're just a normal triplex bus
                  {
                     // This one isn't negated (normal operations)
                     tempPbus =  -bus[indexer].PL[jindex];
                     // @@@ PG and QG is assumed to be zero here @@@ - this may change later (PV busses)
                     tempQbus =  -bus[indexer].QL[jindex];
                  }
                  // end normal triplex bus

                  // Get diagonal contributions - only (& always) 2
                  // Column 1
                  tempIcalcReal += (BA_diag[indexer].Y[jindex][0]).Re() * (bus[indexer].V[0]).Re() 
                     - (BA_diag[indexer].Y[jindex][0]).Im() * (bus[indexer].V[0]).Im();
                  // equation (7), the diag elements of bus admittance matrix
                  tempIcalcImag += (BA_diag[indexer].Y[jindex][0]).Re() * (bus[indexer].V[0]).Im() 
                     + (BA_diag[indexer].Y[jindex][0]).Im() * (bus[indexer].V[0]).Re();
                  // equation (8), the diag elements of bus admittance matrix

                  // Column 2
                  tempIcalcReal += (BA_diag[indexer].Y[jindex][1]).Re() * (bus[indexer].V[1]).Re() 
                     - (BA_diag[indexer].Y[jindex][1]).Im() * (bus[indexer].V[1]).Im();
                  // equation (7), the diag elements of bus admittance matrix
                  tempIcalcImag += (BA_diag[indexer].Y[jindex][1]).Re() * (bus[indexer].V[1]).Im() 
                     + (BA_diag[indexer].Y[jindex][1]).Im() * (bus[indexer].V[1]).Re();
                  // equation (8), the diag elements of bus admittance matrix

                  // Now off diagonals
                  for (kindexer=0; kindexer<(bus[indexer].Link_Table_Size); kindexer++)
                  {
                     // Apply proper index to jindexer (easier to implement this way)
                     jindexer=bus[indexer].Link_Table[kindexer];

                     if (branch[jindexer].from == indexer) // We're the from bus
                     {
                        if ((bus[indexer].phases & 0x20) == 0x20) // SPCT from bus - needs different signage
                        {
                           work_vals_char_0 = jindex*3;

                           // This situation can only be a normal line (triplex will never be the from for another type)
                           // Again only, & always 2 columns (just do them explicitly)
                           // Column 1
                           tempIcalcReal += ((branch[jindexer].Yfrom[work_vals_char_0])).Re() * 
                              (bus[branch[jindexer].to].V[0]).Re() 
                              - ((branch[jindexer].Yfrom[work_vals_char_0])).Im() * 
                              (bus[branch[jindexer].to].V[0]).Im();
                           // equation (7), the off_diag elements of bus admittance matrix are equal to negative value 
                           // of branch admittance
                           tempIcalcImag += ((branch[jindexer].Yfrom[work_vals_char_0])).Re() * 
                              (bus[branch[jindexer].to].V[0]).Im() + 
                              ((branch[jindexer].Yfrom[work_vals_char_0])).Im() * (bus[branch[jindexer].to].V[0]).Re();
                           // equation (8), the off_diag elements of bus admittance matrix are equal to negative value
                           // of branch admittance

                           // Column2
                           tempIcalcReal += ((branch[jindexer].Yfrom[jindex*3+1])).Re() * 
                              (bus[branch[jindexer].to].V[1]).Re() - ((branch[jindexer].Yfrom[jindex*3+1])).Im() * 
                              (bus[branch[jindexer].to].V[1]).Im();
                           // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                           // value of branch admittance
                           tempIcalcImag += ((branch[jindexer].Yfrom[jindex*3+1])).Re() * 
                              (bus[branch[jindexer].to].V[1]).Im() + ((branch[jindexer].Yfrom[jindex*3+1])).Im() * 
                              (bus[branch[jindexer].to].V[1]).Re();
                           // equation (8), the off_diag elements of bus admittance matrix are equal to negative value 
                           // of branch admittance

                        } // End SPCT To bus - from diagonal contributions
                        else // Normal line connection to normal triplex
                        {
                           work_vals_char_0 = jindex*3;
                           // This situation can only be a normal line (triplex will never be the from for another type)
                           // Again only, & always 2 columns (just do them explicitly)
                           // Column 1
                           tempIcalcReal += (-(branch[jindexer].Yfrom[work_vals_char_0])).Re() * 
                              (bus[branch[jindexer].to].V[0]).Re() 
                              - (-(branch[jindexer].Yfrom[work_vals_char_0])).Im() * 
                              (bus[branch[jindexer].to].V[0]).Im();
                           // equation (7), the off_diag elements of bus admittance matrix are equal to negative value
                           // of branch admittance
                           tempIcalcImag += (-(branch[jindexer].Yfrom[work_vals_char_0])).Re() * 
                              (bus[branch[jindexer].to].V[0]).Im() 
                              + (-(branch[jindexer].Yfrom[work_vals_char_0])).Im() * 
                              (bus[branch[jindexer].to].V[0]).Re();
                           // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                           // value of branch admittance

                           // Column2
                           tempIcalcReal += (-(branch[jindexer].Yfrom[jindex*3+1])).Re() * 
                              (bus[branch[jindexer].to].V[1]).Re() - (-(branch[jindexer].Yfrom[jindex*3+1])).Im() * 
                              (bus[branch[jindexer].to].V[1]).Im();
                           // equation (7), the off_diag elements of bus admittance matrix are equal to negative value 
                           // of branch admittance
                           tempIcalcImag += (-(branch[jindexer].Yfrom[jindex*3+1])).Re() * 
                              (bus[branch[jindexer].to].V[1]).Im() + (-(branch[jindexer].Yfrom[jindex*3+1])).Im() * 
                              (bus[branch[jindexer].to].V[1]).Re();
                           // equation (8), the off_diag elements of bus admittance matrix are equal to negative value 
                           // of branch admittance

                        }
                        // end normal triplex from
                     }
                     // end from bus
                     else if (branch[jindexer].to == indexer) // We're the to bus
                     {
                        if (branch[jindexer].v_ratio != 1.0) // Transformer
                        {
                           // Only a single contributor on the from side - figure out how to get to it
                           if ((branch[jindexer].phases & 0x01) == 0x01) // C
                           {
                              temp_index=2;
                           }
                           else if ((branch[jindexer].phases & 0x02) == 0x02) // B
                           {
                              temp_index=1;
                           }
                           else if ((branch[jindexer].phases & 0x04) == 0x04) // A
                           {
                              temp_index=0;
                           }
                           else // How'd we get here!?!
                           {
                              GL_THROW("NR: A split-phase transformer appears to have an invalid phase");
                           }

                           work_vals_char_0 = jindex*3+temp_index;

                           // Perform the update, it only happens for one column (nature of the transformer)
                           tempIcalcReal += (-(branch[jindexer].Yto[work_vals_char_0])).Re() * 
                              (bus[branch[jindexer].from].V[temp_index]).Re() 
                              - (-(branch[jindexer].Yto[work_vals_char_0])).Im() * 
                              (bus[branch[jindexer].from].V[temp_index]).Im();
                           // equation (7), the off_diag elements of bus admittance matrix are equal to negative value 
                           // of branch admittance
                           tempIcalcImag += (-(branch[jindexer].Yto[work_vals_char_0])).Re() * 
                              (bus[branch[jindexer].from].V[temp_index]).Im() + 
                              (-(branch[jindexer].Yto[work_vals_char_0])).Im() * 
                              (bus[branch[jindexer].from].V[temp_index]).Re();
                           // equation (8), the off_diag elements of bus admittance matrix are equal to negative value 
                           // of branch admittance

                        }
                        // end transformer
                        else // Must be a normal line then
                        {
                           if ((bus[indexer].phases & 0x20) == 0x20) // SPCT from bus - needs different signage
                           {
                              work_vals_char_0 = jindex*3;
                              // This case should never really exist, but if someone reverses a secondary or is doing 
                              // meshed secondaries, it might
                              // Again only, & always 2 columns (just do them explicitly)
                              // Column 1
                              tempIcalcReal += ((branch[jindexer].Yto[work_vals_char_0])).Re() * 
                                 (bus[branch[jindexer].from].V[0]).Re() - 
                                 ((branch[jindexer].Yto[work_vals_char_0])).Im() * 
                                 (bus[branch[jindexer].from].V[0]).Im();
                              // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                              tempIcalcImag += ((branch[jindexer].Yto[work_vals_char_0])).Re() * 
                                 (bus[branch[jindexer].from].V[0]).Im() + 
                                 ((branch[jindexer].Yto[work_vals_char_0])).Im() * 
                                 (bus[branch[jindexer].from].V[0]).Re();
                              // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance

                              // Column2
                              tempIcalcReal += ((branch[jindexer].Yto[work_vals_char_0+1])).Re() * 
                                 (bus[branch[jindexer].from].V[1]).Re() - 
                                 ((branch[jindexer].Yto[work_vals_char_0+1])).Im() * 
                                 (bus[branch[jindexer].from].V[1]).Im();
                              // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                              tempIcalcImag += ((branch[jindexer].Yto[work_vals_char_0+1])).Re() * 
                                 (bus[branch[jindexer].from].V[1]).Im() + 
                                 ((branch[jindexer].Yto[work_vals_char_0+1])).Im() * 
                                 (bus[branch[jindexer].from].V[1]).Re();
                              // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                           } // End SPCT To bus - from diagonal contributions
                           else // Normal line connection to normal triplex
                           {
                              work_vals_char_0 = jindex*3;
                              // Again only, & always 2 columns (just do them explicitly)
                              // Column 1
                              tempIcalcReal += (-(branch[jindexer].Yto[work_vals_char_0])).Re() * 
                                 (bus[branch[jindexer].from].V[0]).Re() - 
                                 (-(branch[jindexer].Yto[work_vals_char_0])).Im() * 
                                 (bus[branch[jindexer].from].V[0]).Im();
                              // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                              tempIcalcImag += (-(branch[jindexer].Yto[work_vals_char_0])).Re() * 
                                 (bus[branch[jindexer].from].V[0]).Im() + 
                                 (-(branch[jindexer].Yto[work_vals_char_0])).Im() * 
                                 (bus[branch[jindexer].from].V[0]).Re();
                              // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance

                              // Column2
                              tempIcalcReal += (-(branch[jindexer].Yto[work_vals_char_0+1])).Re() * 
                                 (bus[branch[jindexer].from].V[1]).Re() - 
                                 (-(branch[jindexer].Yto[work_vals_char_0+1])).Im() * 
                                 (bus[branch[jindexer].from].V[1]).Im();
                              // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                              tempIcalcImag += (-(branch[jindexer].Yto[work_vals_char_0+1])).Re() * 
                                 (bus[branch[jindexer].from].V[1]).Im() + 
                                 (-(branch[jindexer].Yto[work_vals_char_0+1])).Im() * 
                                 (bus[branch[jindexer].from].V[1]).Re();
                              // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                           } // End normal triplex connection
                        }
                        // end normal line
                     }
                     // end to bus
                     else // We're nothing
                        ;

                  } // End branch traversion

                  // It's I.  Just a direct conversion (P changed above to I as well)
                  deltaI_NR[2*bus[indexer].Matrix_Loc+ BA_diag[indexer].size + jindex] = tempPbus - tempIcalcReal;
                  deltaI_NR[2*bus[indexer].Matrix_Loc + jindex] = tempQbus - tempIcalcImag;
               } // End split-phase present
               else // Three phase or some variant thereof
               {
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
                        GL_THROW("NR: A voltage index failed to be found.");
                        /*  TROUBLESHOOT
                            While attempting to compute the calculated power current, a voltage index failed to be
                            resolved.  Please submit your code and a bug report via the trac website.
                            */
                     }

                     // Diagonal contributions
                     tempIcalcReal += (BA_diag[indexer].Y[jindex][kindex]).Re() * (bus[indexer].V[temp_index]).Re() - 
                        (BA_diag[indexer].Y[jindex][kindex]).Im() * (bus[indexer].V[temp_index]).Im();
                     // equation (7), the diag elements of bus admittance matrix
                     tempIcalcImag += (BA_diag[indexer].Y[jindex][kindex]).Re() * (bus[indexer].V[temp_index]).Im() + 
                        (BA_diag[indexer].Y[jindex][kindex]).Im() * (bus[indexer].V[temp_index]).Re();
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
                        GL_THROW("NR: A voltage index failed to be found.");
                     }

                     for (kindexer=0; kindexer<(bus[indexer].Link_Table_Size); kindexer++)
                        // Parse through the branch list
                     {
                        // Apply proper index to jindexer (easier to implement this way)
                        jindexer=bus[indexer].Link_Table[kindexer];

                        if (branch[jindexer].from == indexer)
                        {
                           // See if we're a triplex transformer (will only occur on the from side)
                           if ((branch[jindexer].phases & 0x80) == 0x80) // Triplexy
                           {
                              proceed_flag = false;
                              phase_worka = branch[jindexer].phases & 0x07;

                              if (kindex==0) // All of this will only occur the first column iteration
                              {
                                 switch (bus[indexer].phases & 0x07)	{
                                    case 0x01: // C
                                       {
                                          if (phase_worka==0x01)
                                             proceed_flag=true;
                                          break;
                                       }
                                       // end 0x01
                                    case 0x02: // B
                                       {
                                          if (phase_worka==0x02)
                                             proceed_flag=true;
                                          break;
                                       }
                                       // end 0x02
                                    case 0x03: // BC
                                       {
                                          if ((jindex==0) && (phase_worka==0x02)) // First row and is a B
                                             proceed_flag=true;
                                          else if ((jindex==1) && (phase_worka==0x01)) // Second row and is a C
                                             proceed_flag=true;
                                          else
                                             ;
                                          break;
                                       }
                                       // end 0x03
                                    case 0x04: // A
                                       {
                                          if (phase_worka==0x04)
                                             proceed_flag=true;
                                          break;
                                       }
                                       // end 0x04
                                    case 0x05: // AC
                                       {
                                          if ((jindex==0) && (phase_worka==0x04)) // First row and is a A
                                             proceed_flag=true;
                                          else if ((jindex==1) && (phase_worka==0x01)) // Second row and is a C
                                             proceed_flag=true;
                                          else
                                             ;
                                          break;
                                       }
                                       // end 0x05
                                    case 0x06: // AB - shares with ABC
                                    case 0x07: // ABC
                                       {
                                          if ((jindex==0) && (phase_worka==0x04)) // A & first row
                                             proceed_flag=true;
                                          else if ((jindex==1) && (phase_worka==0x02)) // B & second row
                                             proceed_flag=true;
                                          else if ((jindex==2) && (phase_worka==0x01)) // C & third row
                                             proceed_flag=true;
                                          else;
                                          break;
                                       }
                                       // end 0x07
                                 }
                                 // end switch
                              } // End if kindex==0

                              if (proceed_flag)
                              {
                                 work_vals_char_0 = temp_index_b*3;
                                 // Do columns individually
                                 // 1
                                 tempIcalcReal += (-(branch[jindexer].Yfrom[work_vals_char_0])).Re() * 
                                    (bus[branch[jindexer].to].V[0]).Re() - 
                                    (-(branch[jindexer].Yfrom[work_vals_char_0])).Im() * 
                                    (bus[branch[jindexer].to].V[0]).Im();
                                 // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                                 // value of branch admittance
                                 tempIcalcImag += (-(branch[jindexer].Yfrom[work_vals_char_0])).Re() * 
                                    (bus[branch[jindexer].to].V[0]).Im() + 
                                    (-(branch[jindexer].Yfrom[work_vals_char_0])).Im() * 
                                    (bus[branch[jindexer].to].V[0]).Re();
                                 // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                                 // value of branch admittance

                                 // 2
                                 tempIcalcReal += (-(branch[jindexer].Yfrom[work_vals_char_0+1])).Re() * 
                                    (bus[branch[jindexer].to].V[1]).Re() - 
                                    (-(branch[jindexer].Yfrom[work_vals_char_0+1])).Im() * 
                                    (bus[branch[jindexer].to].V[1]).Im();
                                 // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                                 // value of branch admittance
                                 tempIcalcImag += (-(branch[jindexer].Yfrom[work_vals_char_0+1])).Re() * 
                                    (bus[branch[jindexer].to].V[1]).Im() + 
                                    (-(branch[jindexer].Yfrom[work_vals_char_0+1])).Im() * 
                                    (bus[branch[jindexer].to].V[1]).Re();
                                 // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                                 // value of branch admittance

                              }
                           }
                           // end SPCT transformer
                           else // /Must be a standard line
                           {
                              work_vals_char_0 = temp_index_b*3+temp_index;
                              work_vals_double_0 = (-branch[jindexer].Yfrom[work_vals_char_0]).Re();
                              work_vals_double_1 = (-branch[jindexer].Yfrom[work_vals_char_0]).Im();
                              work_vals_double_2 = (bus[branch[jindexer].to].V[temp_index]).Re();
                              work_vals_double_3 = (bus[branch[jindexer].to].V[temp_index]).Im();

                              tempIcalcReal += work_vals_double_0 * work_vals_double_2 - work_vals_double_1 * 
                                 work_vals_double_3;
                              // equation (7), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance
                              tempIcalcImag += work_vals_double_0 * work_vals_double_3 + work_vals_double_1 * 
                                 work_vals_double_2;
                              // equation (8), the off_diag elements of bus admittance matrix are equal to negative 
                              // value of branch admittance

                           }
                           // end standard line

                        }
                        if  (branch[jindexer].to == indexer)
                        {
                           work_vals_char_0 = temp_index_b*3+temp_index;
                           work_vals_double_0 = (-branch[jindexer].Yto[work_vals_char_0]).Re();
                           work_vals_double_1 = (-branch[jindexer].Yto[work_vals_char_0]).Im();
                           work_vals_double_2 = (bus[branch[jindexer].from].V[temp_index]).Re();
                           work_vals_double_3 = (bus[branch[jindexer].from].V[temp_index]).Im();

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
                  work_vals_double_0 = (bus[indexer].V[temp_index_b]).Mag()*(bus[indexer].V[temp_index_b]).Mag();

                  if (work_vals_double_0!=0)
                     // Only normal one (not square), but a zero is still a zero even after that
                  {
                     work_vals_double_1 = (bus[indexer].V[temp_index_b]).Re();
                     work_vals_double_2 = (bus[indexer].V[temp_index_b]).Im();
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
               } // End three-phase or variant thereof
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
                  delta_current[0] = (voltageDel[0] == 0) ? 0 : ~(bus[indexer].S[0]/voltageDel[0]);

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
                  delta_current[1] = (voltageDel[1] == 0) ? 0 : ~(bus[indexer].S[1]/voltageDel[1]);

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
                  delta_current[2] = (voltageDel[2] == 0) ? 0 : ~(bus[indexer].S[2]/voltageDel[2]);

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

                  // Check for "different" children and apply them, as well
                  if ((bus[indexer].phases & 0x10) == 0x10) // We do, so they must be Wye-connected
                  {
                     // Power values
                     undeltacurr[0] += (bus[indexer].V[0] == 0) ? 0 : ~(bus[indexer].extra_var[0]/bus[indexer].V[0]);

                     // Shunt values
                     undeltacurr[0] += bus[indexer].extra_var[3]*bus[indexer].V[0];

                     // Current values
                     undeltacurr[0] += bus[indexer].extra_var[6];
                  }
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[0] = 0.0;
               }

               if ((bus[indexer].phases & 0x02) == 0x02) // Has a phase B
               {
                  undeltacurr[1]=(bus[indexer].I[1]+delta_current[1])-(bus[indexer].I[0]+delta_current[0]);

                  // Check for "different" children and apply them, as well
                  if ((bus[indexer].phases & 0x10) == 0x10) // We do, so they must be Wye-connected
                  {
                     // Power values
                     undeltacurr[1] += (bus[indexer].V[1] == 0) ? 0 : ~(bus[indexer].extra_var[1]/bus[indexer].V[1]);

                     // Shunt values
                     undeltacurr[1] += bus[indexer].extra_var[4]*bus[indexer].V[1];

                     // Current values
                     undeltacurr[1] += bus[indexer].extra_var[7];
                  }
               }
               else
               {
                  // Zero it, just in case
                  undeltacurr[1] = 0.0;
               }


               if ((bus[indexer].phases & 0x01) == 0x01) // Has a phase C
               {
                  undeltacurr[2]=(bus[indexer].I[2]+delta_current[2])-(bus[indexer].I[1]+delta_current[1]);

                  // Check for "different" children and apply them, as well
                  if ((bus[indexer].phases & 0x10) == 0x10) // We do, so they must be Wye-connected
                  {
                     // Power values
                     undeltacurr[2] += (bus[indexer].V[2] == 0) ? 0 : ~(bus[indexer].extra_var[2]/bus[indexer].V[2]);

                     // Shunt values
                     undeltacurr[2] += bus[indexer].extra_var[5]*bus[indexer].V[2];

                     // Current values
                     undeltacurr[2] += bus[indexer].extra_var[8];
                  }
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
                     GL_THROW("NR: A Jacobian update element failed.");
                     // Defined below
                  }

                  if ((bus[indexer].V[temp_index_b]).Mag()!=0)
                  {
                     bus[indexer].Jacob_A[temp_index] = ((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Re() + 
                           (undeltacurr[temp_index_b]).Im() *pow((bus[indexer].V[temp_index_b]).Im(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) + (undeltaimped[temp_index_b]).Im();
                     // second part of equation(37) - no power term needed
                     bus[indexer].Jacob_B[temp_index] = -((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Im() + 
                           (undeltacurr[temp_index_b]).Re() *pow((bus[indexer].V[temp_index_b]).Re(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) - (undeltaimped[temp_index_b]).Re();
                     // second part of equation(38) - no power term needed
                     bus[indexer].Jacob_C[temp_index] =((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Im() - 
                           (undeltacurr[temp_index_b]).Re() *pow((bus[indexer].V[temp_index_b]).Im(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) - (undeltaimped[temp_index_b]).Re();
                     // second part of equation(39) - no power term needed
                     bus[indexer].Jacob_D[temp_index] = ((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Re() - 
                           (undeltacurr[temp_index_b]).Im() *pow((bus[indexer].V[temp_index_b]).Re(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) - (undeltaimped[temp_index_b]).Im();
                     // second part of equation(40) - no power term needed
                  }
                  else
                     // Zero voltage = only impedance is valid (others get divided by VMag, so are IND) - not entirely 
                     // sure how this gets in here anyhow
                  {
                     bus[indexer].Jacob_A[temp_index] = (undeltaimped[temp_index_b]).Im() - 1e-4;
                     // Small offset to avoid singularities (if impedance is zero too)
                     bus[indexer].Jacob_B[temp_index] = -(undeltaimped[temp_index_b]).Re() - 1e-4;
                     bus[indexer].Jacob_C[temp_index] = -(undeltaimped[temp_index_b]).Re() - 1e-4;
                     bus[indexer].Jacob_D[temp_index] = -(undeltaimped[temp_index_b]).Im() - 1e-4;
                  }
               } // End phase traversion
            }
            // end delta-connected load
            else if	((bus[indexer].phases & 0x80) == 0x80) // Split phase computations
            {
               // Convert it all back to current (easiest to handle)
               // Get V12 first
               voltageDel[0] = bus[indexer].V[0] + bus[indexer].V[1];

               // Start with the currents (just put them in)
               temp_current[0] = bus[indexer].I[0];
               temp_current[1] = bus[indexer].I[1];
               temp_current[2] = *bus[indexer].extra_var;
               // current12 is not part of the standard current array

               // Now add in power contributions
               temp_current[0] += bus[indexer].V[0] == 0.0 ? 0.0 : ~(bus[indexer].S[0]/bus[indexer].V[0]);
               temp_current[1] += bus[indexer].V[1] == 0.0 ? 0.0 : ~(bus[indexer].S[1]/bus[indexer].V[1]);
               temp_current[2] += voltageDel[0] == 0.0 ? 0.0 : ~(bus[indexer].S[2]/voltageDel[0]);

               // Last, but not least, admittance/impedance contributions
               temp_current[0] += bus[indexer].Y[0]*bus[indexer].V[0];
               temp_current[1] += bus[indexer].Y[1]*bus[indexer].V[1];
               temp_current[2] += bus[indexer].Y[2]*voltageDel[0];

               // See if we are a house-connected node, if so, adjust and add in those values as well
               if ((bus[indexer].phases & 0x40) == 0x40)
               {
                  // Update phase adjustments
                  temp_store[0].SetPolar(1.0,bus[indexer].V[0].Arg());
                  // Pull phase of V1
                  temp_store[1].SetPolar(1.0,bus[indexer].V[1].Arg());
                  // Pull phase of V2
                  temp_store[2].SetPolar(1.0,voltageDel[0].Arg());
                  // Pull phase of V12

                  // Update these current contributions (use delta current variable, it isn't used in here anyways)
                  delta_current[0] = bus[indexer].house_var[0]/(~temp_store[0]);
                  // Just denominator conjugated to keep math right (rest was conjugated in house)
                  delta_current[1] = bus[indexer].house_var[1]/(~temp_store[1]);
                  delta_current[2] = bus[indexer].house_var[2]/(~temp_store[2]);

                  // Now add it into the current contributions
                  temp_current[0] += delta_current[0];
                  temp_current[1] += delta_current[1];
                  temp_current[2] += delta_current[2];
               } // End house-attached splitphase

               // Convert 'em to line currents - they need to be negated (due to the convention from earlier)
               temp_store[0] = -(temp_current[0] + temp_current[2]);
               temp_store[1] = -(-temp_current[1] - temp_current[2]);

               for (jindex=0; jindex<2; jindex++)
               {
                  if ((bus[indexer].V[jindex]).Mag()!=0) // Only current
                  {
                     bus[indexer].Jacob_A[jindex] = ((bus[indexer].V[jindex]).Re()*(bus[indexer].V[jindex]).Im()*
                           (temp_store[jindex]).Re() + (temp_store[jindex]).Im() *
                           pow((bus[indexer].V[jindex]).Im(),2))/pow((bus[indexer].V[jindex]).Mag(),3);
                     // second part of equation(37)
                     bus[indexer].Jacob_B[jindex] = -((bus[indexer].V[jindex]).Re()*
                           (bus[indexer].V[jindex]).Im()*(temp_store[jindex]).Im() + 
                           (temp_store[jindex]).Re() *pow((bus[indexer].V[jindex]).Re(),2))/
                           pow((bus[indexer].V[jindex]).Mag(),3);
                     // second part of equation(38)
                     bus[indexer].Jacob_C[jindex] =((bus[indexer].V[jindex]).Re()*(bus[indexer].V[jindex]).Im()*
                           (temp_store[jindex]).Im() - (temp_store[jindex]).Re() *
                           pow((bus[indexer].V[jindex]).Im(),2))/pow((bus[indexer].V[jindex]).Mag(),3);
                     // second part of equation(39)
                     bus[indexer].Jacob_D[jindex] = ((bus[indexer].V[jindex]).Re()*(bus[indexer].V[jindex]).Im()*
                           (temp_store[jindex]).Re() - (temp_store[jindex]).Im() *
                           pow((bus[indexer].V[jindex]).Re(),2))/pow((bus[indexer].V[jindex]).Mag(),3);
                     // second part of equation(40)
                  }
                  else
                  {
                     bus[indexer].Jacob_A[jindex]=  -1e-4;
                     // Put very small to avoid singularity issues
                     bus[indexer].Jacob_B[jindex]=  -1e-4;
                     bus[indexer].Jacob_C[jindex]=  -1e-4;
                     bus[indexer].Jacob_D[jindex]=  -1e-4;
                  }
               }

               // Zero the last elements, just to be safe (shouldn't be an issue, but who knows)
               bus[indexer].Jacob_A[2] = 0.0;
               bus[indexer].Jacob_B[2] = 0.0;
               bus[indexer].Jacob_C[2] = 0.0;
               bus[indexer].Jacob_D[2] = 0.0;

            }
            // end split-phase connected
            else // Wye-connected system/load
            {
               // For Wye-connected, only compute and store phases that exist (make top heavy)
               temp_index = -1;
               temp_index_b = -1;

               if ((bus[indexer].phases & 0x10) == 0x10)
                  // "Different" child load - in this case it must be delta - also must be three phase (just because 
                  // that's how I forced it to be implemented)
               {
                  // Calculate all the deltas to wyes in advance (otherwise they'll get repeated)
                  // Make sure phase combinations exist
                  if ((bus[indexer].phases & 0x06) == 0x06) // Has A-B
                  {
                     // Delta voltages
                     voltageDel[0] = bus[indexer].V[0] - bus[indexer].V[1];

                     // Power - put into a current value (iterates less this way)
                     delta_current[0] = (voltageDel[0] == 0) ? 0 : ~(bus[indexer].extra_var[0]/voltageDel[0]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[0] += voltageDel[0] * (bus[indexer].extra_var[3]);
                  }
                  else
                  {
                     // Zero it, for good measure
                     voltageDel[0] = 0.0;
                     delta_current[0] = 0.0;
                  }

                  // Check for BC
                  if ((bus[indexer].phases & 0x03) == 0x03) // Has B-C
                  {
                     // Delta voltages
                     voltageDel[1] = bus[indexer].V[1] - bus[indexer].V[2];

                     // Power - put into a current value (iterates less this way)
                     delta_current[1] = (voltageDel[1] == 0) ? 0 : ~(bus[indexer].extra_var[1]/voltageDel[1]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[1] += voltageDel[1] * (bus[indexer].extra_var[4]);
                  }
                  else
                  {
                     // Zero it, for good measure
                     voltageDel[1] = 0.0;
                     delta_current[1] = 0.0;
                  }

                  // Check for CA
                  if ((bus[indexer].phases & 0x05) == 0x05) // Has C-A
                  {
                     // Delta voltages
                     voltageDel[2] = bus[indexer].V[2] - bus[indexer].V[0];

                     // Power - put into a current value (iterates less this way)
                     delta_current[2] = (voltageDel[2] == 0) ? 0 : ~(bus[indexer].extra_var[2]/voltageDel[2]);

                     // Convert delta connected load to appropriate Wye
                     delta_current[2] += voltageDel[2] * (bus[indexer].extra_var[5]);
                  }
                  else
                  {
                     // Zero it, for good measure
                     voltageDel[2] = 0.0;
                     delta_current[2] = 0.0;
                  }

                  // Convert delta-current into a phase current - reuse temp variable
                  undeltacurr[0]=(bus[indexer].extra_var[6]+delta_current[0])-
                     (bus[indexer].extra_var[8]+delta_current[2]);
                  undeltacurr[1]=(bus[indexer].extra_var[7]+delta_current[1])-
                     (bus[indexer].extra_var[6]+delta_current[0]);
                  undeltacurr[2]=(bus[indexer].extra_var[8]+delta_current[2])-
                     (bus[indexer].extra_var[7]+delta_current[1]);
               }
               else // zero the variable so we don't have excessive ifs
               {
                  undeltacurr[0] = undeltacurr[1] = undeltacurr[2] = 0.0;
                  // Zero it
               }

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
                     GL_THROW("NR: A Jacobian update element failed.");
                     /*  TROUBLESHOOT
                         While attempting to calculate the "dynamic" portions of the
                         Jacobian matrix that encompass attached loads, an update failed to process correctly.
                         Submit you code and a bug report using the trac website.
                         */
                  }

                  if ((bus[indexer].V[temp_index_b]).Mag()!=0)
                  {
                     bus[indexer].Jacob_A[temp_index] = ((bus[indexer].S[temp_index_b]).Im() * 
                           (pow((bus[indexer].V[temp_index_b]).Re(),2) - pow((bus[indexer].V[temp_index_b]).Im(),2)) - 
                           2*(bus[indexer].V[temp_index_b]).Re()*(bus[indexer].V[temp_index_b]).Im()*
                           (bus[indexer].S[temp_index_b]).Re())/pow((bus[indexer].V[temp_index_b]).Mag(),4);
                     // first part of equation(37)
                     bus[indexer].Jacob_A[temp_index] += ((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(bus[indexer].I[temp_index_b]).Re() + 
                           (bus[indexer].I[temp_index_b]).Im() *pow((bus[indexer].V[temp_index_b]).Im(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) + (bus[indexer].Y[temp_index_b]).Im();
                     // second part of equation(37)
                     bus[indexer].Jacob_A[temp_index] += ((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Re() + 
                           (undeltacurr[temp_index_b]).Im() *pow((bus[indexer].V[temp_index_b]).Im(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3);
                     // current part of equation (37) - Handles "different" children

                     bus[indexer].Jacob_B[temp_index] = ((bus[indexer].S[temp_index_b]).Re() * 
                           (pow((bus[indexer].V[temp_index_b]).Re(),2) - 
                            pow((bus[indexer].V[temp_index_b]).Im(),2)) + 
                           2*(bus[indexer].V[temp_index_b]).Re()*(bus[indexer].V[temp_index_b]).Im()*
                           (bus[indexer].S[temp_index_b]).Im())/pow((bus[indexer].V[temp_index_b]).Mag(),4);
                     // first part of equation(38)
                     bus[indexer].Jacob_B[temp_index] += -((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(bus[indexer].I[temp_index_b]).Im() + 
                           (bus[indexer].I[temp_index_b]).Re() *pow((bus[indexer].V[temp_index_b]).Re(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) - (bus[indexer].Y[temp_index_b]).Re();
                     // second part of equation(38)
                     bus[indexer].Jacob_B[temp_index] += -((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Im() + 
                           (undeltacurr[temp_index_b]).Re() *pow((bus[indexer].V[temp_index_b]).Re(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3);
                     // current part of equation(38) - Handles "different" children

                     bus[indexer].Jacob_C[temp_index] = ((bus[indexer].S[temp_index_b]).Re() * 
                           (pow((bus[indexer].V[temp_index_b]).Im(),2) - pow((bus[indexer].V[temp_index_b]).Re(),2)) - 
                           2*(bus[indexer].V[temp_index_b]).Re()*(bus[indexer].V[temp_index_b]).Im()*
                           (bus[indexer].S[temp_index_b]).Im())/pow((bus[indexer].V[temp_index_b]).Mag(),4);
                     // first part of equation(39)
                     bus[indexer].Jacob_C[temp_index] +=((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(bus[indexer].I[temp_index_b]).Im() - 
                           (bus[indexer].I[temp_index_b]).Re() *pow((bus[indexer].V[temp_index_b]).Im(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) - (bus[indexer].Y[temp_index_b]).Re();
                     // second part of equation(39)
                     bus[indexer].Jacob_C[temp_index] +=((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Im() - 
                           (undeltacurr[temp_index_b]).Re() *pow((bus[indexer].V[temp_index_b]).Im(),2))/
                        pow((bus[indexer].V[temp_index_b]).Mag(),3);
                     // Current part of equation(39) - Handles "different" children

                     bus[indexer].Jacob_D[temp_index] = ((bus[indexer].S[temp_index_b]).Im() * 
                           (pow((bus[indexer].V[temp_index_b]).Re(),2) - pow((bus[indexer].V[temp_index_b]).Im(),2)) - 
                           2*(bus[indexer].V[temp_index_b]).Re()*(bus[indexer].V[temp_index_b]).Im()*
                           (bus[indexer].S[temp_index_b]).Re())/pow((bus[indexer].V[temp_index_b]).Mag(),4);
                     // first part of equation(40)
                     bus[indexer].Jacob_D[temp_index] += ((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(bus[indexer].I[temp_index_b]).Re() - 
                           (bus[indexer].I[temp_index_b]).Im() *pow((bus[indexer].V[temp_index_b]).Re(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3) - (bus[indexer].Y[temp_index_b]).Im();
                     // second part of equation(40)
                     bus[indexer].Jacob_D[temp_index] += ((bus[indexer].V[temp_index_b]).Re()*
                           (bus[indexer].V[temp_index_b]).Im()*(undeltacurr[temp_index_b]).Re() - 
                           (undeltacurr[temp_index_b]).Im() *pow((bus[indexer].V[temp_index_b]).Re(),2))/
                           pow((bus[indexer].V[temp_index_b]).Mag(),3);
                     // Current part of equation(40) - Handles "different" children

                  }
                  else
                  {
                     bus[indexer].Jacob_A[temp_index]= (bus[indexer].Y[temp_index_b]).Im() - 1e-4;
                     // Small offset to avoid singularity issues
                     bus[indexer].Jacob_B[temp_index]= -(bus[indexer].Y[temp_index_b]).Re() - 1e-4;
                     bus[indexer].Jacob_C[temp_index]= -(bus[indexer].Y[temp_index_b]).Re() - 1e-4;
                     bus[indexer].Jacob_D[temp_index]= -(bus[indexer].Y[temp_index_b]).Im() - 1e-4;
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
            if  (bus[jindexer].type != 1) // PV bus ignored (for now?)
               size_diag_update += BA_diag[jindexer].size;
            else {}
         }

         if (Y_diag_update == NULL)
         {
            Y_diag_update = (Y_NR *)gl_malloc((4*size_diag_update) *sizeof(Y_NR));
            // Y_diag_update store the row,column and value of the dynamic part of the diagonal PQ bus elements of 
            // 6n*6n Y_NR matrix.

            // Make sure it worked
            if (Y_diag_update == NULL)
               GL_THROW("NR: Failed to allocate memory for one of the necessary matrices");

            // Update maximum size
            max_size_diag_update = size_diag_update;
         }
         else if (size_diag_update > max_size_diag_update) // We've exceeded our limits
         {
            // Disappear the old one
            gl_free(Y_diag_update);

            // Make a new one in its image
            Y_diag_update = (Y_NR *)gl_malloc((4*size_diag_update) *sizeof(Y_NR));

            // Make sure it worked
            if (Y_diag_update == NULL)
               GL_THROW("NR: Failed to allocate memory for one of the necessary matrices");

            // Update the size
            max_size_diag_update = size_diag_update;

            // Flag for a realloc
            NR_realloc_needed = true;
         }

         indexer = 0;
         // Rest positional counter

         for (jindexer=0; jindexer<bus_count; jindexer++) // Parse through bus list
         {
            if (bus[jindexer].type == 2) // Swing bus
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

            if (bus[jindexer].type != 1 && bus[jindexer].type != 2) // No PV or swing (so must be PQ)
            {
               for (jindex=0; jindex<BA_diag[jindexer].size; jindex++)
               {
                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind;
                  Y_diag_update[indexer].Y_value = (BA_diag[jindexer].Y[jindex][jindex]).Im() + 
                     bus[jindexer].Jacob_A[jindex];
                  // Equation(14)
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind + BA_diag[jindexer].size;
                  Y_diag_update[indexer].Y_value = (BA_diag[jindexer].Y[jindex][jindex]).Re() + 
                     bus[jindexer].Jacob_B[jindex];
                  // Equation(15)
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex + BA_diag[jindexer].size;
                  Y_diag_update[indexer].col_ind = 2*bus[jindexer].Matrix_Loc + jindex;
                  Y_diag_update[indexer].Y_value = (BA_diag[jindexer].Y[jindex][jindex]).Re() + 
                     bus[jindexer].Jacob_C[jindex];
                  // Equation(16)
                  indexer += 1;

                  Y_diag_update[indexer].row_ind = 2*bus[jindexer].Matrix_Loc + jindex + BA_diag[jindexer].size;
                  Y_diag_update[indexer].col_ind = Y_diag_update[indexer].row_ind;
                  Y_diag_update[indexer].Y_value = -(BA_diag[jindexer].Y[jindex][jindex]).Im() + 
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
            gl_warning("Empty powerflow connectivity matrix, your system is empty!");
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
            Y_Amatrix = (Y_NR *)gl_malloc((size_Amatrix) *sizeof(Y_NR));
            // Amatrix includes all the elements of Y_offdiag_PQ, Y_diag_fixed and Y_diag_update.

            // Make sure it worked
            if (Y_Amatrix == NULL)
               GL_THROW("NR: Failed to allocate memory for one of the necessary matrices");
         }
         else if (NR_realloc_needed) // If one of the above changed, we changed too
         {
            // Destroy the faulty version
            gl_free(Y_Amatrix);

            // Create a new one that holds our new ampleness
            Y_Amatrix = (Y_NR *)gl_malloc((size_Amatrix) *sizeof(Y_NR));

            // Make sure it worked
            if (Y_Amatrix == NULL)
               GL_THROW("NR: Failed to allocate memory for one of the necessary matrices");
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
            Y_Work_Amatrix = (Y_NR *)gl_malloc(size_Amatrix*sizeof(Y_NR));
            if (Y_Work_Amatrix==NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");
         }
         else if (NR_realloc_needed) // Y_Amatrix was likely resized, so we need it too since we's cousins
         {
            // Get rid of the old
            gl_free(Y_Work_Amatrix);

            // And in with the new
            Y_Work_Amatrix = (Y_NR *)gl_malloc(size_Amatrix*sizeof(Y_NR));
            if (Y_Work_Amatrix==NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");
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
            matrices_LU.a_LU = (double *) gl_malloc(nnz *sizeof(double));
            if (matrices_LU.a_LU==NULL)
            {
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");
               /*  TROUBLESHOOT
                   While attempting to allocate the memory for one of the SuperLU working matrices,
                   an error was encountered and it was not allocated.  Please try again.  If it fails
                   again, please submit your code and a bug report using the trac website.
                   */
            }

            matrices_LU.rows_LU = (int *) gl_malloc(nnz *sizeof(int));
            if (matrices_LU.rows_LU == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            matrices_LU.cols_LU = (int *) gl_malloc((n+1) *sizeof(int));
            if (matrices_LU.cols_LU == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            /* Create the right-hand side matrix B. */
            matrices_LU.rhs_LU = (double *) gl_malloc(m *sizeof(double));
            if (matrices_LU.rhs_LU == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            // /* Set up the arrays for the permutations. */
            perm_r = (int *) gl_malloc(m *sizeof(int));
            if (perm_r == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            perm_c = (int *) gl_malloc(n *sizeof(int));
            if (perm_c == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            // Set up storage pointers - single element, but need to be malloced for some reason
            A_LU.Store = (void *)gl_malloc(sizeof(NCformat));
            if (A_LU.Store == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            B_LU.Store = (void *)gl_malloc(sizeof(DNformat));
            if (B_LU.Store == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

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
            gl_free(matrices_LU.a_LU);
            gl_free(matrices_LU.rows_LU);
            gl_free(matrices_LU.cols_LU);
            gl_free(matrices_LU.rhs_LU);

            // Free up superLU matrices
            gl_free(perm_r);
            gl_free(perm_c);

            /* Set aside space for the arrays. - Copied from above */
            matrices_LU.a_LU = (double *) gl_malloc(nnz *sizeof(double));
            if (matrices_LU.a_LU==NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            matrices_LU.rows_LU = (int *) gl_malloc(nnz *sizeof(int));
            if (matrices_LU.rows_LU == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            matrices_LU.cols_LU = (int *) gl_malloc((n+1) *sizeof(int));
            if (matrices_LU.cols_LU == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            /* Create the right-hand side matrix B. */
            matrices_LU.rhs_LU = (double *) gl_malloc(m *sizeof(double));
            if (matrices_LU.rhs_LU == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            // /* Set up the arrays for the permutations. */
            perm_r = (int *) gl_malloc(m *sizeof(int));
            if (perm_r == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

            perm_c = (int *) gl_malloc(n *sizeof(int));
            if (perm_c == NULL)
               GL_THROW("NR: One of the SuperLU solver matrices failed to allocate");

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
            if (bus[indexer].type != 2)
            {
               // Figure out the offset we need to be for each phase
               if ((bus[indexer].phases & 0x80) == 0x80) // Split phase
               {
                  // Pull the two updates (assume split-phase is always 2)
                  DVConvCheck[0]=complex(sol_LU[2*bus[indexer].Matrix_Loc],sol_LU[(2*bus[indexer].Matrix_Loc+2)]);
                  DVConvCheck[1]=complex(sol_LU[(2*bus[indexer].Matrix_Loc+1)],sol_LU[(2*bus[indexer].Matrix_Loc+3)]);
                  bus[indexer].V[0] += DVConvCheck[0];
                  bus[indexer].V[1] += DVConvCheck[1];
                  // Negative due to convention

                  // Pull off the magnitude (no sense calculating it twice)
                  CurrConvVal=DVConvCheck[0].Mag();
                  if (CurrConvVal > Maxmismatch) // Update our convergence check if it is bigger
                     Maxmismatch=CurrConvVal;

                  if (CurrConvVal > bus[indexer].max_volt_error) // Check for convergence
                     newiter=true;
                  // Flag that a new iteration must occur

                  CurrConvVal=DVConvCheck[1].Mag();
                  if (CurrConvVal > Maxmismatch) // Update our convergence check if it is bigger
                     Maxmismatch=CurrConvVal;

                  if (CurrConvVal > bus[indexer].max_volt_error) // Check for convergence
                     newiter=true;
                  // Flag that a new iteration must occur
               }
               // end split phase update
               else // Not split phase
               {
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
                        GL_THROW("NR: An error occurred indexing voltage updates");
                        /*  TROUBLESHOOT
                            While attempting to create the voltage update indices for the
                            Newton-Raphson solver, an error was encountered.  Please submit
                            your code and a bug report using the trac website.
                            */
                     }

                     DVConvCheck[jindex]=complex(sol_LU[(2*bus[indexer].Matrix_Loc+temp_index)],
                           sol_LU[(2*bus[indexer].Matrix_Loc+BA_diag[indexer].size+temp_index)]);
                     bus[indexer].V[temp_index_b] += DVConvCheck[jindex];

                     // Pull off the magnitude (no sense calculating it twice)
                     CurrConvVal=DVConvCheck[jindex].Mag();
                     if (CurrConvVal > bus[indexer].max_volt_error) // Check for convergence
                        newiter=true;
                     // Flag that a new iteration must occur

                     if (CurrConvVal > Maxmismatch)
                        // See if the current differential is the largest found so far or not
                        Maxmismatch = CurrConvVal;
                     // It is, store it

                  } // End For loop for phase traversion
               } // End not split phase update
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
               gl_verbose("Power flow calculation converges at Iteration %d \n",Iteration+1);
            }
            break;
         }
      } // End iteration loop

      // Check to see how we are ending
      if ((Iteration==NR_iteration_limit) && (newiter==true)) // Reached the limit
      {
         gl_verbose("Max solver mismatch of failed solution %f\n",Maxmismatch);
         return -Iteration;
      }
      else if (info!=0) // failure of computations (singular matrix, etc.)
      {
         // For superLU - 2 = singular matrix it appears - positive values = process errors (singular, etc), negative 
         // values = input argument/syntax error
         gl_verbose("superLU failed out with return value %d",info);

         *bad_computations = true;
         // Flag our output as bad
         return 0;
         // Just return some arbitrary value
      }
      else // Must have converged
         return Iteration;
   }
}
