
load('freqs.mat', 'freqs');
% find the closest value
N=[1 8 64 256 1024];
a=input('Enter frequency value(in Hz) that you wish to generate:  ');
x=0; y=0;
tmp=8000000;
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
display(N(y),'Prescaller');
display(x-1,'OCR0');
display(freqs(y,x),'Frequency going to be generated');


