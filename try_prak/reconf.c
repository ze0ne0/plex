#include<stdio.h>
#include<stdlib.h>
#include<math.h>


int main()
{

	FILE *fp=NULL;
	const int Naccess=32000,thresh=1024;
	unsigned int prevAddress=0,base=0,last_base=0,count=0,last_count=0;
	unsigned int  currAddress=0;
	int diff=0,i,j,new_flag=0;	
	int last_i;
	//--------------------------------------------------


	fp=fopen("file1","r");
	if(fp == NULL)
	{
		perror("File open error\n");
		return -1;
	}			
	

	for(i=0;i<Naccess;i++)
	{
		fscanf(fp,"%x",&currAddress);
	
		if(prevAddress!=0)
		{
			diff=base-currAddress;
			diff=(diff>0)?diff:-diff;

			printf("cur=%d base=%d diff=%d \n",currAddress,base,diff);

			//----------------Phase-Detection------------------------

			if(diff > thresh)
			{
				// YEs
				if(new_flag==0)
				{
					//YES; means new observation starts
					last_base=base;
					last_count=count;
					base=currAddress;
					count=0;new_flag=1;
					printf("New fresh obs starts\n");
				}
				else
				{
					//NO ; Currently one more observation is going on
					printf("1 more obs starts,1 ws going\n");
					base=currAddress;
					count=0;
				}
			}		
			else
			{
				//NO
				count++;
				if(count >= 25 && new_flag==1)
				{
					new_flag=0;
					printf("phase changed from %d to %d \n",last_base,base);
				}
			}
				
			//-------------------------------------------------------
		}else
		{	
			last_base=base=currAddress;
		}
		
		prevAddress=currAddress;
	}
	
	fclose(fp);
	return 0;
}
