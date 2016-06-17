/*****************************************************************************************************************************************
*
*  PROJECT NAME= RADIO FREQUENCY IDENTIFICATION USING ARM
*  PLATEFORM   = ARM BOARD--> AT91RM9200
*		 TOOL     --> WORKBENCH 2.5(WIND RIVER)
*  
*  @uthor : Asif N.
*  PLACE        = CRANES SOFTWARE INTERNATIONAL LTD (VARSITY DIVISION), BANGALORE.
*
*  DATE OF CREATION=       5 DEC 2008    
*  LAST UPDATED = 20 DEC 2008.   
*
*
******************************************************************************************************************************************/
/*---------file inclusion-----*/
/*include<xbdRamDisc.h>*/
#include <msgQLib.h>
#include <semLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ramdrv.h>
#include <string.h>
#include <dosfslib.h>
#include <taskLib.h>
#include <fcntl.h>
/*---------macro definitions------*/
#define HOUR 8
#define MIN 30
#define SEC 0
#define DATE 19
#define SUCCESS 0
#define FAIL -1
#define MAX_TRY 3
#define IN 1
#define OUT 0
/*---------structures and enum declatration--------*/
enum departments { MANAGER, EMBEDDED, VLSI, DSP, VISITOR };
struct mytime
{
	int hour;
	int min;
	int sec;
	int date;
}tm={HOUR, MIN, SEC, DATE};
struct emp
{
	char name[10];
	char id[15];
	unsigned dpt:3;
};
struct logemp
{
	struct emp detail;
	struct mytime login;
	struct mytime logout;
	unsigned status:1;
};
struct visitor
{
	char name[20];
	char e_name[10];
	char ph_no[11];
};
/*----------function declaration---------------*/
/*struct mytime *time_dif(struct mytime *,struct mytime*);*/
int visitor(void);
void main_door(void);
/*void info_show(int);*/
int write_file(void);
void dept_door(void);
void treader(void);
void myclock(void);
/*----------global variable----------*/
int reader_tid,mdoor_tid,ddoor_tid,datab_tid,clock_tid;
MSG_Q_ID mdmqid,ddmqid;
SEM_ID mbsid,dbsid;
/* --------------------start of main programm (programm entry point)-------*/
int kmain()
{	
	
	mdmqid=msgQCreate(5,15,MSG_Q_PRIORITY);
	if(mdmqid==NULL)
	{
		printf("PROBLEM IN CREATION OF MSG QUEUE\n");
		exit(0);
	}
	ddmqid=msgQCreate(2,sizeof(struct emp),MSG_Q_PRIORITY);
	if(ddmqid==NULL)
	{
		printf("PROBLEM IN CREATION OF MSG QUEUE\n");
		exit(0);
	}
	
	mbsid=semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);
	dbsid=semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);
	if(write_file())
	{
		printf("PROBLEM IN CREATION OF DATA BASE\n");
		exit(0);
	}
	
	clock_tid=taskSpawn("tclock",50,VX_NO_STACK_FILL,2000,(FUNCPTR)myclock,0,0,0,0,0,0,0,0,0,0);
	reader_tid=taskSpawn("treader",90,VX_NO_STACK_FILL,2000,(FUNCPTR)treader,0,0,0,0,0,0,0,0,0,0);
	mdoor_tid=taskSpawn("tmdoor",101,VX_NO_STACK_FILL,8000,(FUNCPTR)main_door,0,0,0,0,0,0,0,0,0,0);
	ddoor_tid=taskSpawn("tddoor",101,VX_NO_STACK_FILL,8000,(FUNCPTR)dept_door,0,0,0,0,0,0,0,0,0,0);	
	
	return 0;
}
void myclock(void)
{
	int sec,min,hr,dt,hour=HOUR,minute=MIN,second=SEC;
	for(;;)
	{
		for(hr=hour;hr<24;hr++)
		{
			
			for(min=minute;min<60;min++)
			{
				for(sec=second;sec<60;sec++)
				{
					taskDelay(1*40);
					tm.sec=sec;
				}
				tm.min=min;
				second=0;
				
			}
			tm.hour=hr;
			minute=0;
		}
		tm.date+=1;
		hour=0;
	}
}
void treader(void)
{
	char rid[15],buf,tempbuf[15];
	/*inf fd=open("/tyco/1",O_RDWR,0666);
	ioctl(fd,FIOBAUDRATE,9600);
	while(1)
	{
		ioctl(fd,FIOFLUSH,0);
		for(j=0;j<13;j++)
		{
			read(fd,buf,1);
			tempbuf[j]=buf;
		}
		tempbuf[j++]='\0';
		strcpy(rid,tempbuf);
		msgQSend(mdmqid,rid,14,-1,1);
	}*/
	while(1)
	{
		printf("\n***********************************************\n");
		printf("************   WELCOME TO CSIL   **************\n");
		printf("***********************************************\n");
		printf("\nENTER THE ID(0 to exit, 1111 for visitor)==");

		scanf("%s",rid);

		if(!strcmp(rid,"0"))
		{	
			printf("-----GOOD BYE-----");
			exit(0);
		}
		if(msgQSend(mdmqid,rid,sizeof(rid),NO_WAIT,MSG_PRI_URGENT))
		{
			printf("Message sending error !\n");
		}
		semTake(mbsid,WAIT_FOREVER);
	}	
}
/*void info_show(int info_choice)
{
	FILE *f1;
	char dept[10],status[4];
	struct logemp employee;
	struct visitor vi;
	
	if(info_choice==1)
	{
		f1=fopen("/ASIFN/login.txt","a+");
		printf("\n***************************************************************************************\n");
		printf("----------------             LOGGED IN EMPLOYEE DETAIL               -------------------\n");
		printf("\n**************************************************************************************\n");
		printf(" ---- NAME -----   DEPARTMENT  ----   LOGIN TIME  ----  LOGOUT TIME   ---- STATUS ---\n");
		while(!feof(f1))
		{
			fread(&employee,sizeof(employee),1,f1);
			switch(employee.detail.dpt)
			{
				case 1:strcpy(dept,"MANAGER");break;
				case 2:strcpy(dept,"EMBEDDED");break;
				case 3:strcpy(dept,"VLSI");break;
				case 4:strcpy(dept,"DSP");break;
			}
			switch(employee.status)
			{
				case 0:strcpy(status,"OUT");break;
				case 1:strcpy(status,"IN");break;
			}

			
			printf("     %s       %s     %d:%d:%d date:%d     %d:%d:%d date:%d       ",employee.detail.name,dept,employee.login.hour,employee.login.min,employee.login.sec,employee.login.date,employee.logout.hour,employee.logout.min,employee.logout.sec,employee.logout.date);
			printf("%s  \n\n",status);
		}
	}
}*/
void main_door(void)
{	
	char buf[15];
 	int c_employee=0,c_visitor=0,flag,vflag=1,log_choice,l_flag=1,sflag=1;
	FILE *f1,*f2,*f3;
	int info_choice;
	fpos_t *tempf;
	struct mytime *t_dif; 
	struct emp e; 
	struct logemp le;
	while(1)
	{
		vflag=1;sflag=1;
		if(!msgQReceive(mdmqid,&buf,sizeof(buf),WAIT_FOREVER)){printf("message reading error encountered !\n");}
		f1=fopen("/ASIFN/employee.txt","r");
		if(f1==NULL){printf("read open fails");exit(0);}
	/*	if(!strncmp(buf,"info",4))
		{
			while(info_choice)	
			{	printf("----ENTER YOUR CHOICE----\n1...EMPLOYEE\n2...VISITOR\n0...EXIT\n---->");
				scanf("%d",&info_choice);
				switch(info_choice)
				{
					case 1:
						info_show(info_choice);
						break;
					case 2:
						info_show(info_choice);
						break;
					case 0:
						break;
					default:
						printf("---WRONG CHOICE---\n");
				}
			}
		}*/
	   	while((!feof(f1)) && vflag )
		{
			fread(&e,sizeof(e),1,f1);
			/*printf("in second while");*/
			if(!strcmp(buf,e.id))
			{	
				flag=SUCCESS;
				if(e.dpt==VISITOR)
				{	fclose(f1);
					if(!visitor())
						c_visitor++;
					else
					{
						printf("----SORRY, THE PERSON IS NOT AVAILABLE !----\n------GOOD BYE-----\n");
					}
					vflag=0;
				}
				else
				{	
					printf("-----ENTER YOUR CHOICE----\n1...LOGIN\n2...LOGOUT\n----->  ");
					scanf("%d",&log_choice);
					if(log_choice==2)
					{
						
						f2=fopen("/ASIFN/login.txt","a+");
						
						if(f2==NULL){printf("---YOU HAVE NOT LOGGED IN---\n");}
						fseek(f2,0,SEEK_SET);
						while(!feof(f2))
						{
							fread(&le,sizeof(le),1,f2);
							if(!strcmp(e.id,le.detail.id) && le.status==IN)
							{	
								l_flag=0;
								le.status=OUT;
								le.logout.hour=tm.hour;le.logout.min=tm.min;le.logout.sec=tm.sec;le.logout.date=tm.date;
								fseek(f2,-sizeof(le),SEEK_CUR);
								fwrite(&le,sizeof(le),1,f2);
								fclose(f2);
								c_employee--;
								/*t_dif=time_dif(&l.login,&l.logout);*/
								printf("----YOU HAVE SUCCESSFULLY LOGGED OF----\n");
								printf("\n-----------------------------------------\n");
								printf("---------         GOOD BYE      ----------\n");
								printf("-----------------------------------------\n");
								printf("NO. OF EMPLOYEE(S) PRESENT--%d\n",c_employee);
								printf("NO. OF VISITOR(S) PRESENT---%d\n",c_visitor);
								printf("_________________________________________\n");
								printf("\nYOUR DETAIL:\nNAME= %s  LOGIN  TIME: %d:%d:%d DATE: %d\n",le.detail.name,le.login.hour,le.login.min,le.login.sec,le.login.date);
								printf("             LOGOUT TIME: %d:%d:%d DATE: %d\n",le.logout.hour,le.logout.min,le.logout.sec,le.logout.date);
								printf("_________________________________________\n");
								
								
								
								break;				
							}
							l_flag=1;	
						}
						
						if(l_flag){printf("----YOU HAVE NOT LOGGED IN----\n");fclose(f2);}
						
						break;
					}
					if(log_choice==1)
					{
						
							f2=fopen("/ASIFN/login.txt","a+");
							fseek(f2,0,SEEK_SET);
							while(!feof(f2))
							{
								fread(&le,sizeof(le),1,f2);
								if(!strcmp(buf,le.detail.id))
								{
									if(le.status==IN){printf("already logged in\n");sflag=0;break;}
								}
							}
							if(sflag==0){fclose(f2);break;}
							fclose(f2);
							c_employee++;
							printf("\n-----------------------------------------\n");
							printf("-------   WELCOME TO CSIL      ----------\n");
							printf("-----------------------------------------\n");
							printf("NO. OF EMPLOYEE(S) PRESENT--%d\n",c_employee);
							printf("NO. OF VISITOR(S) PRESENT---%d\n",c_visitor);
							printf("_________________________________________\n");
							
							strcpy(le.detail.name,e.name);strcpy(le.detail.id,e.id);le.detail.dpt=e.dpt;
							le.login.hour=tm.hour;le.login.min=tm.min;le.login.sec=tm.sec;le.login.date=tm.date;
							le.logout.hour=0;le.logout.min=0;le.logout.sec=0;le.logout.date=0;le.status=IN;
							printf("%d:%d:%d\n",le.login.hour,le.login.min,le.login.sec);
							printf("\nYOUR DETAIL:\nNAME= %s  LOGIN TIME: %d:%d:%d DATE: %d\n",le.detail.name,le.login.hour,le.login.min,le.login.sec,le.login.date);
							printf("_________________________________________\n");
							f2=fopen("/ASIFN/login.txt","a+");
							fwrite(&le,sizeof(struct logemp),1,f2);
							fclose(f2);
						
					
					
							if(msgQSend(ddmqid,(char*)&e,sizeof(e),NO_WAIT,MSG_PRI_URGENT))
							{
								printf("PROBLEM IN MSG SENDING2\n");
							}
							semTake(dbsid,WAIT_FOREVER);
							break;
					}
				}
			}
			else
				flag=FAIL;				
		}
	if(flag){printf("*****-WRONG ID-*****\n");}
	fclose(f1);
	semGive(mbsid);
	}
} 
/*struct mytime *time_dif(struct mytime *lin,struct mytime *lout)
{
	int t_sec_lin,t_sec_lout,t_sec_dif;
	t_sec_lin=((lin->hour)*60*60)+((lin->min)*60)+lin->sec;
	t_sec_lout=((lout->hour)*60*60)+((lout->min)*60)+lout->sec;
	t_sec_dif=t_sec_out-t_sec_in;
	
}*/
void dept_door(void)
{
	struct emp ed;
	char id[15];
	int n_try=MAX_TRY,choice=0,br_flag=1;
	while(1)
	{
		msgQReceive(ddmqid,&ed,sizeof(struct emp),WAIT_FOREVER);
		while((n_try--) && (br_flag))
		{       
			printf("SELECT DEPARTMENT\n1...EMBEDDED\n2...VLSI...\n3...DSP\n0...EXIT\n------>");
			scanf("%d",&choice);
			if(choice==0){goto END;}
			if((choice==1 || choice==2 || choice==3))
			{
				printf("ENTER YOUR ID--->");
				scanf("%s",id);
			}
			
			switch(choice)
			{
				case 1:
					if((ed.dpt==EMBEDDED)||(ed.dpt==MANAGER))
					{
						if(strncmp(id,ed.id,4))
						{  
							printf("----WRONG ID---- %d LOGIN ATTEMPTS REMAINING\n",n_try);
							break;
						}
						printf("HELLO %s !! WELCOME TO EMBEDDED DEPARTMENT\n",ed.name);
						n_try=3;
						goto END;
				 	}
				 	else
				  	     printf("WRONG DEPARTMENT %d LOGIN ATTEMPTS REMAINING\n",n_try);
				  	break;

				case 2:
					 if((ed.dpt==VLSI)||(ed.dpt==MANAGER))
					 {
					       if(strcmp(id,ed.id))
						{       printf("----WRONG ID---- %d LOGIN ATTEMPTS REMAINING\n",n_try);
							break;
						}
						printf("HELLO %s WELLCOME TO VLSI DEPARTMENT\n",ed.name);
						n_try=3;
						goto END;
				 	}
			   	     else
				   	   printf("WRONG DEPARTMENT %d LOGIN ATTEMPTS REMAINING\n",n_try);
			    	    break;
		       
			 	case 3:
				    	if((ed.dpt==DSP)||(ed.dpt==MANAGER))
					  {
						if(strcmp(id,ed.id)){printf("----WRONG ID---- %d LOGIN ATTEMPTS REMAINING\n",n_try);break;}
						printf("HELLO %s WELLCOME TO DSP DEPARTMENT\n",ed.name);
						n_try=3;
						goto END;
					}
					 else
					       printf("WRONG DEPARTMENT %d LOGIN ATTEMPTS REMAINING\n",n_try);
				        break;

		        	default:
				       printf("WRONG CHOICE %d LOGIN ATTEMPTS REMAINING\n",n_try);

			 }
		}
END:		semGive(dbsid);
	}
}
int write_file(void)
{
 	/*devise_t xbd;*/       
	BLK_DEV *ptr;
	FILE *f1;
	struct emp emp1;
	/*xbd=sbdRamDiskDevCreate(512,512,0,"/ASIFN");
	hrtsDiskFormat("/ASIFN",500,0,0,0,0);
	*/
        ptr = ramDevCreate(0,512,400,400,0);
     	dosFsMkfs("/ASIFN",ptr);
	
	f1=fopen("/ASIFN/employee.txt","w");
	if(f1==NULL){printf("write open fails");return(FAIL);}

	strcpy(emp1.name,"Asif");strcpy(emp1.id,"140068f3f07f");emp1.dpt=MANAGER;
	fwrite(&emp1,sizeof(emp1),1,f1);
	
	strcpy(emp1.name,"Sankar");strcpy(emp1.id,"1400689a7294");emp1.dpt=EMBEDDED;
	fwrite(&emp1,sizeof(emp1),1,f1);
	
	strcpy(emp1.name,"Roopesh");strcpy(emp1.id,"140068c02995");emp1.dpt=VLSI;
	fwrite(&emp1,sizeof(emp1),1,f1);
	
	strcpy(emp1.name,"John");strcpy(emp1.id,"140068cda011");emp1.dpt=EMBEDDED;
	fwrite(&emp1,sizeof(emp1),1,f1);
	
	strcpy(emp1.name,"Christopher");strcpy(emp1.id,"5678");emp1.dpt=VLSI;
	fwrite(&emp1,sizeof(emp1),1,f1);
	
	strcpy(emp1.name,"Hashim");strcpy(emp1.id,"6789");emp1.dpt=DSP;
	fwrite(&emp1,sizeof(emp1),1,f1);

	strcpy(emp1.name,"Visitor");strcpy(emp1.id,"1111");emp1.dpt=VISITOR;
	fwrite(&emp1,sizeof(emp1),1,f1);
	/*fprintf(f1,"%s %d %d\n","mani",2345,1);
	fprintf(f1,"%s %d %d\n","sagar",3456,2);
	fprintf(f1,"%s %d %d\n","kiran",4567,1);
	fprintf(f1,"%s %d %d\n","chitra",5678,2);
	fprintf(f1,"%s %d %d\n","hashim",6789,3);*/
	fclose(f1);
	
	return(SUCCESS);
}
int visitor(void)
{
	FILE *f3,*f2;
	struct visitor v;
	struct logemp le;
	taskLock();
	printf("ENTER YOUR INFORMATION\n");
	printf("NAME---");
	scanf("%s",v.name);
	printf("NAME OF EMPLOYEE YOU WANT TO MEET----");
	scanf("%s",v.e_name);
	v.e_name[0]=toupper(v.e_name[0]);
	printf("YOUR PHONE NUMBER---");
	scanf("%s",v.ph_no);
	f3=fopen("/ASIFN/login.txt","r");
	if(f3==NULL){printf("open fails");}
	fseek(f3,0,SEEK_SET);
	taskUnlock();
	while(!feof(f3))
	{
		fread(&le,sizeof(le),1,f3);
		printf("%s\n",le.detail.name);
		if(!strncmp(le.detail.name,v.e_name,4) && (le.status==IN))
		{
			f2=fopen("/ASIFN/visitor.txt","a+");
			fwrite(&v,sizeof(v),1,f2);
			printf("\n------ HELLO AND WELCOME TO CSIL %s  --------\n",v.name);
			return SUCCESS;
		}
	}
	return FAIL;
}