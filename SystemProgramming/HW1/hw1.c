/*

	*System Programming-HW01
	*Author : Oguzcan Pamuk
	*Number : 111044053

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <errno.h>

	/*String Size'i veren fonksiyon*/
	int findSize(FILE *fpInp,char *strFileName);
	/*Asil isi yapan fonksiyon-MyGrep*/
	int findString(FILE *fpInp,char *strFileName,int intStrSize,char *argv);
	/*Usage*/
	void usage();

int main(int argc,char *argv[]){
	/*input file pointer*/
	FILE *fpInp;
	/*Toplam bulunan string sayisi*/
	int intTotalNumber=0;
	/*String Size*/
	int intStrSize=0;

	/*Arguman kontrolu*/
	/*Program ismi-txt file-aranacak string
	  3 arguman olmasi gerekiyor*/
	if(argc!=3){
		if(argc>3)
			printf("Fazla Argüman\n");
		else
			printf("Eksik Argüman\n");
			
		printf("---Programin dogru kullanim sekli---\n");
		usage();
		exit(0);
	}	

	/*dosyayi aciyoruz*/
	/*kontrollerimizi yapiyoruz*/
	if((fpInp=fopen(argv[1],"r"))==NULL){
		printf("Dosya Hatasi : %s\n",strerror(errno));
		exit(0);
	}
	
	/*dosyayi kapatiyoruz*/
	fclose(fpInp);

	/*Size'ı belirliyoruz*/
	intStrSize=findSize(fpInp,argv[1]);
	/*printf("size : %d",intStrSize);*/

	intTotalNumber=findString(fpInp,argv[1],intStrSize,argv[2]);

	/*Kac kez bulundugunu ekrana basiyoruz*/
	printf("Kelimenin text dosyasindaki toplam sayisi : %d\n",intTotalNumber);

	return 0;
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
	
	int findString(FILE *fpInp,char *strFileName,int intStrSize,char *argv){
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
						printf("Line : %d\n",intLineNumber);
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
	void usage(){
		printf("gcc -c hw1.c\n");
		printf("gcc -o exe hw1.o\n");
		printf("./exe input.txt string\n");
	}

