#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int degOfN(int n){
	int temp=n;
	int deg=0;
	int i=0;
	for(i=0;temp>1;i++){
		temp/2;
	}
	deg=i;
	return deg;
}

float ** sum(float **A, float **B, int n ){
	int deg=degOfN(n);
	for(int i=0;i<deg;i++){
		
	
	


