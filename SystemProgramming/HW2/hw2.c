/*

	*System Programming-HW02
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

/*Bir dosya isminin alabilecegi maksimum size*/
#define SIZE 256

#define SIZE_PRINT 300

	/*String Size'i veren fonksiyon*/
	int findSize(FILE *fpInp,char *strFileName);
	/*Asil isi yapan fonksiyon-MyGrep*/
	int findString(FILE *fpInp,char *strFileName,int intStrSize,char *argv,FILE *fpOut);
	/*Dosya islemleri*/
	void dosyaIslemleri(char *strDirName,char *strFindString,FILE *fpOut,int intCounter);
	/*klasor mu txt mi*/
	int findFileOrTxt(char *strDirName);
	/*Print Console*/
	void printConsole(FILE *fpInp,int intPrintSize);
	/*Usage*/
	void usage();

int main(int argc,char *argv[]){

	/*File Pointer*/
	FILE *fp;
	/*Print Size*/
	int intPrintSize=0;
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
	
	/*ciktilar dosyaya yazilacak*/
	/*kontrollerimizi yapiyoruz*/
	if((fp=fopen("output.txt","w"))==NULL){
		printf("Dosya Hatasi : %s\n",strerror(errno));
		exit(0);
	}
	
	/* ./exe dirname -g string -l number*/
	if(strcmp(argv[2],"-g")==0){
		/*-l den sonra sayi yerine string yazilirsa*/
		if(atoi(argv[5])<=0){
			printf("-l'den sonra integer sayi giriniz\n");
			printf("integer olarak 0 girmeyiniz dongu calismaz\n");
			printf("negatif sayi girmeyiniz\n");
			exit(0);
		}
		else
			intPrintSize=atoi(argv[5]);

		dosyaIslemleri(argv[1],argv[3],fp,intCounter);	
	}

	/* ./exe dirname -l string -g number*/
	if(strcmp(argv[2],"-l")==0){
		/*-l den sonra sayi yerine string yazilirsa*/
		if(atoi(argv[3])<=0){
			printf("-l'den sonra integer sayi giriniz\n");
			printf("integer olarak 0 girmeyiniz dongu calismaz\n");
			printf("negatif sayi girmeyiniz\n");
			exit(0);
		}
		else
			intPrintSize=atoi(argv[3]);

		dosyaIslemleri(argv[1],argv[5],fp,intCounter);	
	}
	/*Dosyayi Kapatiyoruz*/
	fclose(fp);

	/*Ekrana Basma*/
        /*dosyayi aciyoruz*/
        /*kontrollerimizi yapiyoruz*/
	if((fp=fopen("output.txt","r"))==NULL){
 		printf("Dosya Hatasi : %s\n",strerror(errno));
		exit(0);
	}

  	printConsole(fp,intPrintSize);

	/*Dosya ile isimiz bitti siliyoruz*/			   
	if(remove("output.txt")==-1){
		printf("Dosya silme basarisiz\n");
		exit(0);
	}
	/*Dosyayi Kapatiyoruz*/
	fclose(fp);
	return 0;
}

	void dosyaIslemleri(char *strDirName,char *strFindString,FILE *fpOut,int intCounter){
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
	
		/*Dosyayi aciyoruz*/
		dpInp=opendir(strDirName);
		
		if (dpInp==NULL) {
			perror("opendir");
			exit(1);
		}
	
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
				/*klasor varsa ona child process acmasini engeller*/
				if(findFileOrTxt(strFname)==1){
					if ((pid = fork()) == -1) {
						perror("Cannot fork :\n");
						exit(1);
					}
					if(pid == 0){
						intStrSize=findSize(fpInp,strFname);
						findString(fpInp,strFname,intStrSize,strFindString,fpOut);
						fprintf(fpOut,"Process pid :%d -- Dosya uzantisi :%s\n",getpid(),strFname);
						exit(0);
					}
					else{
						wait(NULL);
					}
				}
				else 
					dosyaIslemleri(strFname,strFindString,fpOut,intCounter);
				free(strFname);
					
			}	
		}
				
		/*Dosyayi Kapatiyoruz*/
		closedir(dpInp);
		/*Pointeri free yapiyoruz*/
		
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
	
	int findString(FILE *fpInp,char *strFileName,int intStrSize,char *argv,FILE *fpOut){
		/*satir numarasi*/
		int intLineNumber=1;
		/*counter*/
		int intCounter1=0,intCounter2=0,intTotalNumber=0;
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
						fprintf(fpOut,"Line : %d\n",intLineNumber);
						intTotalNumber++;
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
	
		/*Kac kez bulundugunu donduruyoruz*/		
		return (intTotalNumber);
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

	void printConsole(FILE *fpInp,int intPrintSize){
		/*Line uzunlugu 256 karakterden uzun olmuyor*/
		char strTemp[SIZE_PRINT];
		int intCounter=0;
		/*Komut degiskeni*/
		char chrKomut;
	
		for(intCounter=0 ;intCounter<intPrintSize ;intCounter++){
			if(fgets(strTemp,100,fpInp)!=NULL) 
				printf("%s",strTemp);
			else
				break;
			if(intCounter==intPrintSize-1){
				scanf("%c",&chrKomut); 
				if(chrKomut=='q')
					break;
				else 
					intCounter=-1;
			}
		}

	}
	void usage(){
		printf("gcc -c hw2.c\n");
		printf("gcc -o exe hw2.o\n");
		printf("./exe dirname -g string -l number\n");
		printf("Ya da\n");
		printf("./exe dirname -l number -g string\n");
		printf("Ekran ciktisi bolumunde q yaparsaniz cikar\n");
		printf("Enter a basarsaniz devam eder\n");	 
	}




