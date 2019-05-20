

int fun()
{
	/*write code here.*/

	/* Wander up the stack in integer pointers look for the number 10 & change it to 20.*/ 
	int i; 
	int *p=&i; 
	int j; 
	for(j=0;*(p+j)!=10;j++); 
	/* Stack Frame size is j int pointers. */ 
	*(p+j)=20; 
} 

}
int main()
{
	int i=10;
	fun();
	printf("%d",i);
}
//change the value of the i without changing code of the main function, assign 20 to i ?


