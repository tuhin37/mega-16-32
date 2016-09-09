
for f=1:length(allFreqs)
    dat = allFreqs(1,f);
   
    for i=1:7
        for j=1:256
           if r_freq(i,j) == dat
               disp(N(i));
               disp(j-1);
               allFreqs(2,f)=N(i);
               allFreqs(3,f)=j-1;
              
           end
        end
    end
end