N=[1 8 64 256 1024];
F=zeros(953,3);
for l=1:length(C)
    a=C(l);
    x=0; y=0;
    tmp=10000000;
    for i=1:5
       for j=1:256
           diff=abs(a-freqs(i,j));
           if(diff<=tmp)
               tmp=diff;
               x=j;
               y=i;
           end
       end
    end
    F(l,:)=[C(l) N(y) x-1];
    %display(N(y),'Prescaller');
    %display(x,'OCR0');
end