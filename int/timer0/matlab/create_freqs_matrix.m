% generate freqs matrix
clear all
close all
clc
freqs=zeros(5,256);
N=[1 8 64 256 1024];
for i=1:length(N)
   OCR0=(0:255);
   f=(16000000)./(2*N(i)*(1+OCR0));
   freqs(i,:)=f;
   stem(f);
   hold on
end
