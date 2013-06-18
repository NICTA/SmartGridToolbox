function S = S_of_V(V, Y)
   S = conj(conj(V) .* (Y * V));
end
