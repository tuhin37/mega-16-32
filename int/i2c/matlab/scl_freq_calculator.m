for k=1:832
    tmp = freq(1,k); 
    for j=1:4
        for i=1:256
            f = SCL(j,i);
            if(f == tmp);
                % TWPS value
                freq(2,k) = j-1;
                % TWBR value
                freq(3,k) = i-1;
            end
        end
    end
end