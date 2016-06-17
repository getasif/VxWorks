/*
*RFID Code implemented on ARM BOARD--> AT91RM9200
*
* TOOL --> VxWorks 5.2, WORKBENCH 2.5(WIND RIVER)
*
* 
*@uthor -> Asif N
*
*/


#include <stdio.h>
#include <ramdrv.h>
#include <dosfslib.h>
#include <tasklib.h>
#include <string.h>
#include <msgqlib.h>
#include <ticklib.h>
#include <wdlib.h>
#include <loglib.h>

struct Timer			
{
	int sec;
	int min;
	int hour;
}TIM;

typedef struct Entrytime			
{
	int sec;
	int min;
	int hour;
}ET;

typedef struct Employee
{
	
	char rfid[20];
	char Name[20];
	char Department[20];
	char Entrypoint[20];
	int Position;
	ET Timein;
	ET Timeout;
	

}EMP;

typedef struct Visitor
{
	char Name[20];
	char Address[40];
	char Whomtomeet[20];
	char HisDepartment[20];
	char Entrypoint[20];
	ET Timein;
	ET Timeout;
}VIS;



#define EMPLOYEE 0;
#define MANAGER 1;
#define ADMIN 2;


FILE *fp,*fp1,*fp2,*fp3;
BLK_DEV *ramptr,*ramptr1;
WIND_TCB *ptcb1,*ptcb2,*ptcb3;
MSG_Q_ID server_msg,employee_msg,visitor_msg; 
WDOG_ID wid;
char accessdoor[30];
int server,visitor,employee,swip,entrypoint,Timerupdate1;
int Display_emp,Display_vis,Display_empin,Display_empout;


void server_entry(void);
void employee_entry(void);
void visitor_entry(void);
void Entrypoint(void);
void Swip(void);
void wd_handler(int arg);
void Timerupdate(void);
int compareEntries(EMP *,EMP *);
void display_emp(void);
void display_vis(void);
void display_empin(void);
void display_empout(void);


int kmain(void)
{
	EMP *empl1,*empl2,*empl3,*empl4,*empl5;
	int enter;

	empl1=(EMP *)malloc(sizeof(EMP));
	empl2=(EMP *)malloc(sizeof(EMP));
	empl3=(EMP *)malloc(sizeof(EMP));
	empl4=(EMP *)malloc(sizeof(EMP));
	empl5=(EMP *)malloc(sizeof(EMP));
	
	server_msg=msgQCreate(10,sizeof(EMP),MSG_Q_PRIORITY);
	employee_msg=msgQCreate(10,sizeof(EMP),MSG_Q_PRIORITY);
	visitor_msg=msgQCreate(10,sizeof(VIS),MSG_Q_PRIORITY);

	ramDrv();
	ramptr=ramDevCreate(0,512,200,2000,0);
	
	dosFsMkfs("/MYDEV",ramptr);
	
	
	fp=fopen("/MYDEV/emp.txt","wb+");
	/*if(fp==NULL)
	{
		printf("error in fp\n");
	}
	
	else
	{
		printf("fp=%d\n",(int)fp);
	}*/
	
	fp1=fopen("/MYDEV/vis.txt","wb+");
	/*if(fp1==NULL)
	{
		printf("error in fp1\n");
	}
	else
	{
		printf("fp1=%d\n",(int)fp1);
	}*/

	ramDrv();
	ramptr1=ramDevCreate(0,512,200,200,0);
	dosFsMkfs("/MYDEV1",ramptr1);
	

	fp2=fopen("/MYDEV1/empin.txt","wb+");
	/*if(fp2==NULL)
	{
		printf("error in fp2\n");
	}
	else
	{
		printf("fp2=%d\n",(int)fp2);
	}*/
	
	fp3=fopen("/MYDEV1/empout.txt","wb+");
	/*if(fp3==NULL)
	{
		printf("error in fp3\n");
	}
	else
	{
		printf("fp3=%d\n",(int)fp3);
	}*/

	printf("ENTER THE PLACE YOU ARE GOING TO INSTALL THE RFID SENSOR\n");
	printf("1.EMBEDDED,2.VLSI,3.COMPUTER SCIENCE ,4.DSP, 5.Electrical,6.MAINDOOR\n");
	scanf("%d",&enter);

	switch(enter)
	{
		case 1:
			printf("inside embedded\n");
			strcpy(accessdoor,"Embedded Systems");
			break;
		
		case 2:
			printf("inside Vlsi\n");
			strcpy(accessdoor,"Vlsi");	
			break;
		case 3:
			printf("inside computer\n");
			strcpy(accessdoor,"Computer Science");
			break;

		case 4:
			printf("inside Dsp\n");
			strcpy(accessdoor,"Dsp");
			break;
		case 5:
			printf("inside Electrical\n");
			strcpy(accessdoor,"Electrical");
			break;
		case 6:
			printf("inside MAINDOOR\n");
			strcpy(accessdoor,"maindoor");
			break;
		default:
			printf("wrong entry\n");

			break;
	}
	
	printf("please enter TIME setup\n");
	
	TIM.sec=0;
	printf("enter HOUR..\n");
	scanf("%d",&TIM.hour);
	printf("enter MINUTE..\n");
	scanf("%d",&TIM.min);
	
	

	Timerupdate1=taskSpawn("ttimer",70,0,2000,(FUNCPTR)Timerupdate,0,0,0,0,0,0,0,0,0,0);

	strcpy(empl1->rfid,"1111000011111");
	strcpy(empl1->Name,"asif");
	strcpy(empl1->Department,"Embedded Systems");
	strcpy(empl1->Entrypoint,"");
	empl1->Position=0;
	empl1->Timein.sec=0;
	empl1->Timein.min=0;
	empl1->Timein.hour=0;
	empl1->Timeout.sec=0;
	empl1->Timeout.min=0;
	empl1->Timeout.hour=0;
	fwrite(empl1,sizeof(EMP),1,fp);
	
	strcpy(empl2->rfid,"1111000011110");
	strcpy(empl2->Name,"syed");
	strcpy(empl2->Department,"Vlsi");
	strcpy(empl2->Entrypoint,"");
	empl2->Position=0;
	empl2->Timein.sec=0;
	empl2->Timein.min=0;
	empl2->Timein.hour=0;
	empl2->Timeout.sec=0;
	empl2->Timeout.min=0;
	empl2->Timeout.hour=0;
	fwrite(empl2,sizeof(EMP),1,fp);
	
	strcpy(empl3->rfid,"1111000011101");
	strcpy(empl3->Name,"sankar");
	strcpy(empl3->Department,"Dsp");
	strcpy(empl3->Entrypoint,"");
	empl3->Position=0;
	empl3->Timein.sec=0;
	empl3->Timein.min=0;
	empl3->Timein.hour=0;
	empl3->Timeout.sec=0;
	empl3->Timeout.min=0;
	empl3->Timeout.hour=0;
	fwrite(empl3,sizeof(EMP),1,fp);
	
	strcpy(empl4->rfid,"1111000011011");
	strcpy(empl4->Name,"john");
	strcpy(empl4->Department,"Computer Science");
	strcpy(empl4->Entrypoint,"");
	empl4->Position=0;
	empl4->Timein.sec=0;
	empl4->Timein.min=0;
	empl4->Timein.hour=0;
	empl4->Timeout.sec=0;
	empl4->Timeout.min=0;
	empl4->Timeout.hour=0;
	fwrite(empl4,sizeof(EMP),1,fp);

	strcpy(empl5->rfid,"1111000010111");
	strcpy(empl5->Name,"joseph");
	strcpy(empl5->Department,"Electrical");
	strcpy(empl5->Entrypoint,"");
	empl5->Position=0;
	empl5->Timein.sec=0;
	empl5->Timein.min=0;
	empl5->Timein.hour=0;
	empl5->Timeout.sec=0;
	empl5->Timeout.min=0;
	empl5->Timeout.hour=0;
	fwrite(empl5,sizeof(EMP),1,fp);
	
	
	
	
	entrypoint=taskSpawn("tentrypoint",90,0,2000,(FUNCPTR)Entrypoint,0,0,0,0,0,0,0,0,0,0);
		
	
	/*fclose(fp);
	fclose(fp1);*/
	
	

	return 0;
	
}

void Timerupdate(void)
{
	TIM.sec++;
	taskDelay(60);

	if(TIM.sec==59)
	{
		TIM.min++;
		TIM.sec=0;
		if(TIM.min==60)
		{	
			TIM.hour++;
			TIM.min=0;
			if(TIM.hour==24)
			{
				TIM.hour=0;
			}
		}
	}
taskRestart(0);

}



void Entrypoint(void)
{

	VIS *vis;
	int vis_or_emp;/* if vis_or_emp is 1,then employee*/ 
	vis=(VIS *)malloc(sizeof(VIS));
	
	
	while(1)
	{	 
		wid=wdCreate();
		wdStart(wid,120,(FUNCPTR)wd_handler,99999);
		printf("***************************************************************************\n");
		printf("1.If you are employee press 1 and swip your card\n 2.visitor press 2 \n3.display emp_details\n");
		printf("4.display vis_details\n 5.display emp_login\n6.display emp_logout\n7.shut down\n");
		scanf("%d",&vis_or_emp);
		wdCancel(wid);
		
		switch(vis_or_emp)
		{
			case 1:
			
				swip=taskSpawn("tswip",90,0,2000,(FUNCPTR)Swip,0,0,0,0,0,0,0,0,0,0);
				taskSuspend(0);
				break;				
		
			case 2:
			
				visitor=taskSpawn("tvisitor",80,0,2000,(FUNCPTR)visitor_entry,0,0,0,0,0,0,0,0,0,0);	 
				strcpy(vis->Name,"guest");
				strcpy(vis->Address,"bangalore");
				strcpy(vis->Whomtomeet,"asif");
				strcpy(vis->HisDepartment,"Embedded Systems");
				strcpy(vis->Entrypoint,accessdoor);
				vis->Timein.sec=0;
				vis->Timein.min=0;
				vis->Timein.hour=0;

				msgQSend(visitor_msg,(char *)vis,sizeof(VIS),-1,1);
			
				break;
			
			case 3:
				Display_emp=taskSpawn("tdisemp",80,0,2000,(FUNCPTR)display_emp,0,0,0,0,0,0,0,0,0,0);
				
				break;
			case 4:
				Display_vis=taskSpawn("tdisvis",80,0,2000,(FUNCPTR)display_vis,0,0,0,0,0,0,0,0,0,0);
				
				break;
			case 5:
				Display_empin=taskSpawn("tdisempin",80,0,2000,(FUNCPTR)display_empin,0,0,0,0,0,0,0,0,0,0);
				
				break;
			case 6:
				Display_empout=taskSpawn("tdisempout",80,0,2000,(FUNCPTR)display_empout,0,0,0,0,0,0,0,0,0,0);
				
				break;
			case 7:
				exit(0);
			default:
				printf("WRONG ENTRY PLS ENTER THE CORRECT OPTION\n");						
				break;

		}

	}

}


void wd_handler(int arg)
{
	if(arg==0)
	{
		exit(1);
	
	}
	printf("***************************************************************************\n");
	printf("1.If you are employee press 1 and swip your card\n 2.visitor press 2 \n3.display emp_details\n");
	printf("4.display vis_deatails\n 5.display emp_login\n6.display emp_logout\n7.shut down\n");
	wdStart(wid,120,(FUNCPTR)wd_handler,--arg);
	
}
	

void Swip(void)
{
	EMP *emp,*emp1,*emp2,*emp3,*emp4,*emp5;
	char buff[13];
	int checkbit=0;
	int totalsizeoffile1=0,max1,max2,i;
	emp=(EMP *)malloc(sizeof(EMP));
	emp1=(EMP *)malloc(sizeof(EMP));
	emp2=(EMP *)malloc(sizeof(EMP));
	emp3=(EMP *)malloc(sizeof(EMP));	
	emp4=(EMP *)malloc(sizeof(EMP));
	emp5=(EMP *)malloc(sizeof(EMP));
		
	/*fd=open("/tyco/0",O_RDWR,0);
	ioctl(fd,FIOBAUDRATE,9600);
	
	
	
		for(i=13;i--;0)
		{
			read(fd,&buff[i],1);
		}*/
	printf("Enter your id:");
	scanf("%s",buff);
	fseek(fp,0,SEEK_END);
	totalsizeoffile1=ftell(fp);
	max2=totalsizeoffile1/sizeof(EMP);
	fseek(fp,0,SEEK_SET);
	/*printf("\nmax2=%d\n",max2);*/
	
	
	for(i=0;i<max2;i++)
	{
		fread(emp4,sizeof(EMP),1,fp);
		/*printf("%s\n",emp4->rfid);
		printf("%s\n",emp4->Name);
		printf("%s\n",emp4->Department);
		printf("%s\n",emp4->Entrypoint);
		printf("%d\n",emp4->Position);*/
		if(!strcmp(emp4->rfid,buff))
		{
			checkbit=1;
			strcpy(emp3->rfid,emp4->rfid);
			strcpy(emp3->Name,emp4->Name);
			strcpy(emp3->Department,emp4->Department);
			strcpy(emp3->Entrypoint,accessdoor);
			emp3->Position=emp4->Position;
			emp3->Timein.sec=0;
			emp3->Timein.min=0;
			emp3->Timein.hour=0;
			emp3->Timeout.sec=0;
			emp3->Timeout.min=0;
			emp3->Timeout.hour=0;
		
			break;
		}
		
	
	}
		
	fseek(fp,0,SEEK_END);
	totalsizeoffile1=ftell(fp);
	max2=totalsizeoffile1/sizeof(EMP);
	fseek(fp,0,SEEK_SET);
	/*printf("\nmax2=%d\n",max2);*/
	
	
	for(i=0;i<max2;i++)
	{
		fread(emp1,sizeof(EMP),1,fp);
		/*printf("%s\n",emp1->rfid);
		printf("%s\n",emp1->Name);
		printf("%s\n",emp1->Department);
		printf("%s\n",emp1->Entrypoint);
		printf("*****%s\n",buff);
		printf("%d\n",emp1->Position);*/
		if(!strcmp(emp1->rfid,buff))
		{
			
			strcpy(emp->rfid,emp1->rfid);
			strcpy(emp->Name,emp1->Name);
			strcpy(emp->Department,emp1->Department);
			strcpy(emp->Entrypoint,accessdoor);
			emp->Position=emp1->Position;
			emp->Timein.sec=TIM.sec;
			emp->Timein.min=TIM.min;
			emp->Timein.hour=TIM.hour;
			emp->Timeout.sec=0;
			emp->Timeout.min=0;
			emp->Timeout.hour=0;
			
			
			fseek(fp2,0,SEEK_END);
			totalsizeoffile1=ftell(fp2);
			max1=totalsizeoffile1/sizeof(EMP);
			fseek(fp2,0,SEEK_SET);
			printf("max1=%d\n",max1);
			if(max1==0)
			{
				printf("your log in time is  %d.%d.%d\n",emp->Timein.hour,emp->Timein.min,emp->Timein.sec);	
				fwrite(emp,sizeof(EMP),1,fp2);
				break;
				
			}
			else
			{
				printf("i am in else\n");	
				for(i=0;i<max1;i++)
				{
					fread(emp2,sizeof(EMP),1,fp2);
					printf("down the fread\n");
					if(!strcmp(emp1->rfid,emp2->rfid))
					{	printf("i am in read\n");
						strcpy(emp5->rfid,emp1->rfid);
						strcpy(emp5->Name,emp1->Name);
						strcpy(emp5->Department,emp1->Department);
						strcpy(emp5->Entrypoint,"Embedded Systemss");
						emp5->Position=emp1->Position;
						fread(emp1,sizeof(EMP),1,fp2);
						emp5->Timeout.sec=TIM.sec;
						emp5->Timeout.min=TIM.min;
						emp5->Timeout.hour=TIM.hour;
						printf("your log out time is  %d.%d.%d\n",emp->Timein.hour,emp->Timein.min,emp->Timein.sec);
						fwrite(emp5,sizeof(EMP),1,fp3);
						break;
						
					}	
					else
					{
						printf("your log in time is  %d.%d.%d\n",emp->Timein.hour,emp->Timein.min,emp->Timein.sec);	
						fwrite(emp,sizeof(EMP),1,fp2);
						
						
					}
					
				}
			break;	
			}
			
	}
				
		
	}	
	
		
		if(checkbit!=1)
		{
			printf("Your Card is not a valid one\n");
			taskActivate(entrypoint);
		}
		else
		{
			checkbit=0;
			
			
		}
			
	employee=taskSpawn("temployee",80,0,2000,(FUNCPTR)employee_entry,0,0,0,0,0,0,0,0,0,0);	
		
		/*printf("%s\n",emp3->rfid);
		printf("%s\n",emp3->Name);
		printf("%s\n",emp3->Department);*/

	switch(emp->Position)
	{
	
		case 0:
			msgQSend(employee_msg,(char *)emp3,sizeof(EMP),-1,1);
			break;
			/*taskActivate(employee);*/	
		case 1:
						
			msgQSend(employee_msg,(char *)emp3,sizeof(EMP),-1,1);
			break;
			/*taskActivate(employee);*/		
		case 2:
			server=taskSpawn("tserver",80,0,2000,(FUNCPTR)server_entry,0,0,0,0,0,0,0,0,0,0);
			msgQSend(server_msg,(char *)emp3,sizeof(EMP),-1,1);
			break;
			/*taskActivate(server);*/		
		/*default:
			printf("your RFID access card is not a valid one\n");*/
			
	}

}
		
	

void server_entry(void)
{
	EMP *emp1,*emp2;
	emp1=(EMP *)malloc(sizeof(EMP));
	emp2=(EMP *)malloc(sizeof(EMP));	
	msgQReceive(server_msg,(char *)emp2,sizeof(EMP),-1);	
	strcpy(emp1->rfid,emp2->rfid);
	strcpy(emp1->Name,emp2->Name);
	strcpy(emp1->Department,emp2->Department);
	strcpy(emp1->Entrypoint,emp2->Entrypoint);
	emp1->Position=emp2->Position;
	emp1->Timein=emp2->Timein;

	fwrite(emp1,sizeof(EMP),1,fp);

}

void visitor_entry(void)
{
	VIS *vis1,*vis;	
	
	vis=(VIS *)malloc(sizeof(VIS));
	vis1=(VIS *)malloc(sizeof(VIS));
	
	msgQReceive(visitor_msg,(char *)vis1,sizeof(VIS),-1);
	taskSuspend(entrypoint);
		
	/*strcpy(vis1->Name,vis->Name);
	strcpy(vis1->Address,vis->Address);
	strcpy(vis1->Whomtomeet,vis->Whomtomeet);
	strcpy(vis1->HisDepartment,vis->HisDepartment);
	strcpy(vis1->Entrypoint,vis->Entrypoint);
	vis1->Timein=vis->Timein;*/

	if(!strcmp(vis1->Entrypoint,"maindoor"))
	{
		printf("Go and meet receptionist for further details\n");
	}
	else
	{
		printf("You are visitor,so you are not allowed to enter %s department\n",accessdoor);
	}

	fwrite(vis1,sizeof(VIS),1,fp1);
	taskActivate(entrypoint);
	
}

void employee_entry(void)
{
	EMP *emp1,*emp2;
	int i,max,totalsizeoffile,check=0,matchfound=0;
	emp1=(EMP *)malloc(sizeof(EMP));
	emp2=(EMP *)malloc(sizeof(EMP));	
	msgQReceive(employee_msg,(char *)emp2,sizeof(EMP),-1);



	
	fseek(fp,0,SEEK_END);
	totalsizeoffile=ftell(fp);
	max=totalsizeoffile/sizeof(EMP);
	fseek(fp,0,SEEK_SET);
	/*printf("max=%d\n",max);*/
	
	
	for(i=0;i<max;i++)
	{
		fread(emp1,sizeof(EMP),1,fp);
	
	
		check=compareEntries(emp1,emp2);
		if(check==1)
		{
			matchfound=1;
			break;
		}
	}
	
	if(matchfound==0)
	{
		printf("%s is your department so you are not allowed to enter %s department\n",emp2->Department,emp2->Entrypoint);
		printf("You are not manager to enter every department \n");
	}
	else
	{
		printf("have a nice day\n");
		printf("You are allowed to enter %s department\n",emp2->Entrypoint);
	}
	
	
	taskActivate(entrypoint);
}


int compareEntries(EMP *employee1,EMP *employee2)
{
	
		
	int rfidcmp=0,Namecmp=0,ifmanager=0,entry=0,entryallowed=0;
	
	
	if(!strcmp(employee1->rfid,employee2->rfid))
	{
		rfidcmp=1;
	}
	if(!strcmp(employee1->Name,employee2->Name))
	{
		Namecmp=1;
	}
	if(employee1->Position==1)
	{	
		if(employee2->Position==1)
		{
			ifmanager=1;
		}
	}
	if(!strcmp(employee2->Entrypoint,"maindoor"))
	{
		entry=1;
	}
	if(!strcmp(employee2->Entrypoint,employee1->Department))
	{
		entry=1;
	}
	
	entryallowed=rfidcmp && Namecmp && (ifmanager || entry);

	return (entryallowed);

}

void display_emp(void)
{
	int totalsizeoffile1,max2,i;
	EMP *emp1;
	emp1=(EMP *)malloc(sizeof(EMP));

	fseek(fp,0,SEEK_END);
	totalsizeoffile1=ftell(fp);
	max2=totalsizeoffile1/sizeof(EMP);
	fseek(fp,0,SEEK_SET);
	/*printf("\nmax2=%d\n",max2);*/
	
	
	for(i=0;i<max2;i++)
	{
		fread(emp1,sizeof(EMP),1,fp);
		printf("***************************************************************************\n");
		printf("RFID:%s\n",emp1->rfid);
		printf("NAME:%s\n",emp1->Name);
		printf("DEPARTMENT:%s\n",emp1->Department);
		/*printf("%s\n",emp1->Entrypoint);*/
		switch(emp1->Position)
		{
	
			case 0:
				printf("Employee of %s\n",emp1->Department);
				break;
					
			case 1:
						
				printf("MANAGER\n");
				break;
						
			case 2:
				printf("ADMINISTRATOR\n");
				break;
						
			/*default:
				printf("your RFID access card is not a valid one\n");*/
			
		}

			
	}
	
}

void display_vis(void)
{
	int totalsizeoffile1,max2,i;
	VIS *vis;
	vis=(VIS *)malloc(sizeof(VIS));	


	fseek(fp1,0,SEEK_END);
	totalsizeoffile1=ftell(fp1);
	max2=totalsizeoffile1/sizeof(EMP);
	fseek(fp1,0,SEEK_SET);
	/*printf("\nmax2=%d\n",max2);*/
	
	
	for(i=0;i<max2;i++)
	{
		fread(vis,sizeof(VIS),1,fp1);
		printf("***************************************************************************\n");
		printf("Name:%s\n",vis->Name);
		printf("Address:%s\n",vis->Address);
		printf("He wanted to visit %s of %s department\n",vis->Whomtomeet,vis->HisDepartment);
		
	}

}

void display_empin(void)
{
	int totalsizeoffile1,max2,i;
	EMP *emp1;
	emp1=(EMP *)malloc(sizeof(EMP));

	fseek(fp2,0,SEEK_END);
	totalsizeoffile1=ftell(fp2);
	max2=totalsizeoffile1/sizeof(EMP);
	fseek(fp2,0,SEEK_SET);
	/*printf("\nmax2=%d\n",max2);*/
	
	
	for(i=0;i<max2;i++)
	{
		fread(emp1,sizeof(EMP),1,fp2);
		printf("***************************************************************************\n");
		printf("RFID:%s\n",emp1->rfid);
		printf("NAME:%s\n",emp1->Name);
		printf("DEPARTMENT:%s\n",emp1->Department);
		printf("log in time is  %d.%d.%d\n",emp1->Timein.hour,emp1->Timein.min,emp1->Timein.sec);
	}
	
}

void display_empout(void)
{
	int totalsizeoffile1,max2,i;
	EMP *emp1;
	emp1=(EMP *)malloc(sizeof(EMP));

	fseek(fp3,0,SEEK_END);
	totalsizeoffile1=ftell(fp3);
	max2=totalsizeoffile1/sizeof(EMP);
	fseek(fp3,0,SEEK_SET);
	/*printf("\nmax2=%d\n",max2);*/
	
	
	for(i=0;i<max2;i++)
	{
		fread(emp1,sizeof(EMP),1,fp3);
		printf("***************************************************************************\n");
		printf("RFID:%s\n",emp1->rfid);
		printf("NAME:%s\n",emp1->Name);
		printf("DEPARTMENT:%s\n",emp1->Department);
		printf("log out time is  %d.%d.%d\n",emp1->Timeout.hour,emp1->Timeout.min,emp1->Timeout.sec);
	}
	
}		