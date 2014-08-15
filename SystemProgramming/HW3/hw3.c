/*

	*System Programming-HW03
	*Author : Oguzcan Pamuk
	*Number : 111044053

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


/*Bir dosya isminin alabilecegi maksimum size*/
#define SIZE 30000

int intCountFile=0;
	/*String Size'i veren fonksiyon*/
	int findSize(FILE *fpInp,char *strFileName);
	/*Asil isi yapan fonksiyon-MyGrep*/
	void findString(FILE *fpInp,char *strFileName,int intStrSize,char *argv,int *pipeWrite,char* strRead);
	/*Dosya islemleri*/
	void dosyaIslemleri(char *strDirName,char *strFindString,int intCounter,int intPrint);
	/*klasor mu txt mi*/
	int findFileOrTxt(char *strDirName);
	/*Print Console*/
	void printConsole(int *pipeRead,int intPrintSize,char *strWrite);
	/*Ctr-C yakalama*/
	void catchCtrlC(int signal);
	/*Usage*/
	void usage();

int main(int argc,char *argv[]){

	/*malloc ile yer acmada kullanilacak her dosya okudugunda +1 yapacak
	  kullanacagim fonksiyon recursive oldugu icin disarda 0layip fonksiyona
	  gonderiyorum*/
	int intCounter=0;
	/*Arguman kontrolu*/
	if(argc!=6){
		if(argc>6)
			printf("Fazla Argüman\n");
		else
			printf("Eksik Argüman\n");

		printf("---Programin dogru kullanim sekli---\n");
		usage();
		exit(0);
	}
	if(atoi(argv[5])<=0){
		printf("-l'den sonra integer sayi giriniz\n");
		printf("integer olarak 0 girmeyiniz dongu calismaz\n");
		printf("negatif sayi girmeyiniz\n");
		exit(0);
	}
	if(strcmp(argv[2],"-g")){
		printf("-g yerine yanlis birsey yazdiniz\n");
		exit(0);
	}

	if(strcmp(argv[4],"-l")){
		printf("-l yerine yanlis birsey yazdiniz\n");
		exit(0);
	}
	
	
	/*dosya islemleri yapiliyor*/
	dosyaIslemleri(argv[1],argv[3],intCounter,atoi(argv[5]));
	
  
	return 0;
}

	void dosyaIslemleri(char *strDirName,char *strFindString,int intCounter,int intPrint){
		/*dosya pointer*/
		DIR *dpInp;
		/*dosya info*/
		struct dirent *dinfo;
		/*file pointer*/
		FILE *fpInp;
		/*String Size*/
		int intStrSize;
		/*Dosya ismini tutan string*/
		char *strFname;
		/*fork karsilastirmasinda kullaniyorum*/
		pid_t pid;
		char strWrite[SIZE];
		int intCountPrint=0;
		/*Pipe olusturuldu*/
		int pipedes[2];
		/*Her dosya icin 1 read 1 write pipe olarak gorev yapacak*/
		int *pipeRead=(int*)malloc(sizeof(int));
		int *pipeWrite=(int*)malloc(sizeof(int));
		/*Dosyayi aciyoruz*/
		dpInp=opendir(strDirName);
		
		if (dpInp==NULL) {
			perror("opendir");
			exit(1);
		}
		/*ctr+c yakalama*/
			signal(SIGINT,catchCtrlC);
		/*Dosya bilgileri*/
		while ((dinfo=readdir(dpInp)) != NULL){
			
			if(strcmp(dinfo->d_name,".")!=0 && strcmp(dinfo->d_name,"..")!=0){
				intCounter++;
				/*printf("%s\n", dinfo->d_name);*/
				/*dosyayi gormesi icin dosya ismine uzantisi eklendi*/
				strFname=(char*)malloc(intCounter*SIZE*sizeof(char));
				strcpy(strFname,strDirName);
				strcat(strFname,"/");
				strcat(strFname,dinfo->d_name);
				/*Her seferinde uzerine ekleyerek yer acmasi icin realloc yaptim*/
				pipeRead=(int*)realloc(pipeRead,(intCountFile+1)*sizeof(int));
				pipeWrite=(int*)realloc(pipeWrite,(intCountFile+1)*sizeof(int));
				/*pipe kontrolu*/
				if (pipe(pipedes)<0) {
					fprintf(stderr, "Cannot create pipe\n");
					exit(1);
				}
				/*kullanilacak pipelarin gosterecegi yerler*/
				pipeRead[intCountFile] = pipedes[0];
    				pipeWrite[intCountFile] = pipedes[1];
				
				if(findFileOrTxt(strFname)==1){
					if ((pid = fork()) == -1) {
						perror("Cannot fork :\n");
						exit(1);
					}
					if(pid == 0){
						intStrSize=findSize(fpInp,strFname);
						close(pipeRead[intCountFile]);
						findString(fpInp,strFname,intStrSize,strFindString,&pipeWrite[intCountFile],strWrite);
						close(pipeWrite[intCountFile]);
						exit(0);	
					}
					else{	
						close(pipeWrite[intCountFile]);
						printConsole(&pipeRead[intCountFile],intPrint,strWrite);
						close(pipeRead[intCountFile]);	
					}
					
					intCountFile++;
				}
				else{
					dosyaIslemleri(strFname,strFindString,intCounter,intPrint);
				}
				/*Pointeri free yapiyoruz*/
				free(strFname);
			}
		}
		/*Dosyayi Kapatiyoruz*/
		closedir(dpInp);	
	
		/*Pointeri free yapiyoruz*/
		free(pipeRead);
		free(pipeWrite);		
	}

	int findSize(FILE *fpInp,char *strFileName){
		/*Programin patlamamasi icin size'ı bulalim*/
		/* /n yapmadan tum satirlari toplayip size yapiyorum
	  	   bunun nedeni her ihtimale karsi programin patlamamasi */
		/*okunan karakter sayisi*/
		int intCounter=0;
		/*fscanf icin karakter degiskeni*/
		char chrCharacter;
		/*Dosyayi aciyoruz*/
		fpInp=fopen(strFileName,"r");
		
		/*Dosya sonuna kadar okumasini istiyoruz*/
		while(fscanf(fpInp,"%c",&chrCharacter)!=EOF){
			intCounter++;
		}
		/*Dosyayi kapatiyoruz*/
		fclose(fpInp);

		/*Size'i donduruyoruz*/
		return intCounter;
	}
	
	void findString(FILE *fpInp,char *strFileName,int intStrSize,char *argv,int *pipeWrite,char* strWrite){
		/*satir numarasi*/
		int intLineNumber=1;
		/*counter*/
		int intCounter1=0;
		int intCounter2=0;
		/*text line'i alacak string*/
		char *strLine=(char*)malloc(intStrSize*sizeof(char));
		
		/*Dosyayi aciyoruz*/
		fpInp=fopen(strFileName,"r");
	
		/*Memory Allocate Kontrolu*/
		if(strLine==NULL){
        		printf("Error : Memory not allocated\n");
        		exit(0);
    		}
		/*dosya okuma*/
		/*fgets ile satir satir okuyorum*/
		while(fgets(strLine,intStrSize,fpInp)!=0){
			
			/*Satiri okuduktan sonra karakter karakter ariyorum*/
			for(intCounter1=0;intCounter1<strlen(strLine);intCounter1++){
				/*ilk karakteri buldugu zaman buradakileri yapiyor*/
				if(strLine[intCounter1]==argv[0]){
					/*sonraki karakterlerde var mi kontrolu*/
					for(intCounter2=1;intCounter2<strlen(argv);intCounter2++){
						/*diger karakterler yoksa donguden cikar*/
						if(strLine[intCounter1+intCounter2]!=argv[intCounter2])
							break;
					}
					/*tum karakterler varsa line numarasini basar
					  toplam sayiya +1 ekler*/
					if(intCounter2==strlen(argv)){
						sprintf(strWrite,"Line : %d--Process pid :%d -- Dosya uzantisi :%s"
						,intLineNumber,getpid(),strFileName);
						/*printf("%s",strWrite);*/
						write(*pipeWrite,strWrite,SIZE+1);
					}
				}
				
			}
			/*her satir sonunda satir numarasini +1 artiriyorum*/
			intLineNumber++;
		}		
		/*dosyayi kapatiyoruz*/
		fclose(fpInp);

		/*Free strLine*/
		free(strLine);
	}
	
	int findFileOrTxt(char *strDirName){
		/*klasor mu txt mi*/
		/*dosya pointer*/
		DIR *dpTemp;
		int intStatus;
		dpTemp=opendir(strDirName);
		if (dpTemp==NULL)
			intStatus=1;
		else{
			intStatus=0;
			closedir(dpTemp);
		}
		return intStatus;
	}


	void catchCtrlC(int signal){
		 printf("Catch Ctrl-C signal %d\n",signal);
		 kill(0,SIGKILL);
	}
	void printConsole(int *pipeRead,int intPrintSize,char *strWrite){
		/*Dosyalarin counterlari ortak tutulmasi icin static yaptim*/
		int static intCounter=0;
		/*Komut degiskeni*/
		char chrKomut=' ';
		for(intCounter ;intCounter<intPrintSize ;intCounter++){
			if(read(*pipeRead,strWrite,SIZE+1)>0){
				printf("%s\n",strWrite);
			}
			else
				break;
			
			if(intCounter==intPrintSize-1){
				printf("Devam etmek icin enter'a,cikmak icin q+enter'a basiniz:\n");
				scanf("%c",&chrKomut); 
				if(chrKomut=='q'){
					intCounter=intPrintSize+1;
					return;
				}
				else 
					intCounter=-1;
			}
		

		}

	}

	void usage(){
		printf("gcc -c hw03.c\n");
		printf("gcc -o exe hw03.o\n");
		printf("./exe dirname -g string -l number\n"); 
	}




