function new_ids = permute_ids(ids)
    p = randperm(size(ids));
    new_ids = ids(p);
    printf('size = %d\n', size(new_ids, 1));
    printf('[');
    printf('%d, ', new_ids(1:end-1));
    printf('%d]\n', new_ids(end));
end
