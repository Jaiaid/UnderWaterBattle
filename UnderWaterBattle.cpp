# include"iGraphics.h"
#include<stdlib.h>
#include<time.h>

struct fish{     //structure of fish in the game
	int species; //determines which picture will be showed
	int fishint; //determines from which distance player will be sensed by fish
	double x;    // x position
	double y;    // y position
	double vx;   // x axis velocity
	double vy;   // y axis velocity
};

struct diver{    //structure of player
	int pllf;    //player life
	int harpfl;  //harpoon existance 0 if not exist
	double ply;  //y axis position 
	double plvy; //y axis velocity
	double plair;//air amount
	double harpx;//harpoon x axis position
	double harpy;//harpoon y axis position
};

struct fish fishlist[3];//array of 3 fish
struct diver pl;        //player structure declaration


int fishsizex[4]={128,64,64,32}; //array of fishsize towards x according to species no.
int fishsizey[4]={32,30,64,32}; //array of fishsize towards y according to species no. 
int t1,t2,t3;                  //timers name
int chno=0;                    //position of character while save name input
int scrst=0;                   //which screen to show
int subst=0;                   //determines which part of scrst=4
int picflag=0;                 // to change player image
int bubble=0;                  //determine bubble existance

double pointer[2][3]={{300,330,300},{365,374,383}};//position of pointer(a filled triangle)
double bx,by;                                      //bubble position

char timestring[5]={'2',':','0','0','\0'};         //string to show time
char score[4]={' ',' ','0','\0'};                  //string to show score
char bestscore[11]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'}; //string to store best score
char save[3][5]={"    ","    ","    "};            //strings to store savedgame name while program on 
char specieslist[4][15]={"fig/Shark.bmp","fig/anglerfish.bmp","fig/octopus.bmp","fig/fish.bmp"};//array of file name to show image

time_t ini,final,final1,ini1;  //variables to determine real time change

FILE *sg;               //File pointer to read/write file


void savegame(FILE *fp,int offset,int index) //save game in a file
{
	fseek(fp,offset,SEEK_SET);
	fwrite(save[index],5,1,fp);
	fwrite(timestring,5,1,fp);
	fwrite(score,4,1,fp);
	fwrite(fishlist,sizeof(fishlist),1,fp);
	fwrite(&pl,sizeof(pl),1,fp);
}

int loadgame(FILE *fp,int offset)           //load game from file
{
	fseek(fp,offset+5,SEEK_SET);
	if(1>fread(timestring,5,1,fp)) return 0;
	if(1>fread(score,4,1,fp)) return 0;
	if(1>fread(fishlist,sizeof(fishlist),1,fp)) return 0;
	if(1>fread(&pl,sizeof(pl),1,fp)) return 0;
	return 1;
}

void picflip()                              //change player image to show movement 
{
	if(picflag) picflag=0;
	else picflag=1;
}

void pointermover(int move)                 //control pointer movement.move==1 while upward move==-1 while downward
{
	if(scrst==0)                            //control the pointer in cover screen
	{
		if(pointer[1][0]==365)              //while the pointer is in highest
		{
			if(move==1)
			{
				pointer[1][0]-=150;
				pointer[1][1]-=150;
				pointer[1][2]-=150;
			}
			else 
			{
				pointer[1][0]-=50;
				pointer[1][1]-=50;
				pointer[1][2]-=50;
			}
		}
		else if(pointer[1][0]==215)        //while lowest
		{
			if(move==1)
			{
				pointer[1][0]+=50;
				pointer[1][1]+=50;
				pointer[1][2]+=50;
			}
			else 
			{
				pointer[1][0]+=150;
				pointer[1][1]+=150;
				pointer[1][2]+=150;
			}
		}
		else if(pointer[1][0]==265 || pointer[1][0]==315) //other two position
		{
			pointer[1][0]+=50*move;
			pointer[1][1]+=50*move;
			pointer[1][2]+=50*move;
		}
	}
	else if(scrst==4)                                     //pointer movement in save/load menu and others
	{
		if(pointer[1][0]==265)                            //while pointer is in lowest
		{
			if(move==-1)
			{
				pointer[1][0]+=100;
				pointer[1][1]+=100;
				pointer[1][2]+=100;
			}
			else
			{
				pointer[1][0]+=50;
				pointer[1][1]+=50;
				pointer[1][2]+=50;
			}
		}
		else if(pointer[1][0]==365)                      //while in highest
		{
			if(move==-1)
			{
				pointer[1][0]-=50;
				pointer[1][1]-=50;
				pointer[1][2]-=50;
			}
			else
			{
				pointer[1][0]-=100;
				pointer[1][1]-=100;
				pointer[1][2]-=100;
			}
		}
		else if(pointer[1][0]==315)                      //middle
		{
			if(move==-1)
			{
				pointer[1][0]-=50;
				pointer[1][1]-=50;
				pointer[1][2]-=50;
			}
			else
			{
				pointer[1][0]+=50;
				pointer[1][1]+=50;
				pointer[1][2]+=50;
			}
		}
	}
}

void gameengine()        //control process during gameplay
{
	if(scrst==1)         //do the neccesary initialization to start the play
	{
		scrst=2;         
		subst=0;
		iPauseTimer(t1);
		iResumeTimer(t2);
		iResumeTimer(t3);
		ini=time(NULL);
		ini1=time(NULL);
	}
	if(scrst==2)        //necessary change at gameplay
	{
		if(bx<0) bubble=0;    //bubble existance

		if((timestring[0]=='0' && timestring[2]=='0' && timestring[3]=='0') || pl.pllf<0 || pl.plair<0 ) //check whether game is over
		{
			scrst=3;      //go to win/lose displaying screen
			bubble=0;     //make bubble existance 0
			iPauseTimer(t2); //pausetimer of fish,player&bubble movement
			iPauseTimer(t3); ////pausetimer of player leg movement
			if((sg=fopen("score","rb+"))==NULL) sg=fopen("score","wb+"); //open score file to see if there is new best score
			if(pl.plair>0 && pl.pllf>0)                                  //compare best score with score if it's won game 
			{
				if(1>fread(bestscore,11,1,sg))                           //check if new empty file is created means there was no best
				{
					scrst=5;                                             //go to name save screen
					bestscore[7]=score[0];                               //necessary initialization to save bestscore in file
					bestscore[8]=score[1];
					bestscore[9]=score[2];
					bestscore[4]='\0';
					chno=0;                                              //to take name input
				}
				else if((bestscore[7]-48)*100+(bestscore[8]-48)*10+(bestscore[9]-48)<(score[0]-48)*100+(score[1]-48)*10+(score[2]-48))//if there was best then compare with present score
				{
					scrst=5;
					bestscore[7]=score[0];                                //necessary initialization to save bestscore in file
					bestscore[8]=score[1];
					bestscore[9]=score[2];
					bestscore[4]='\0';
					chno=0;												  //to take name input
				}
			}
			if(scrst==3) fclose(sg);                                      //if save isn't needed close file
		}

		int l;
		for(l=0;l<3;l++)                                                  //to process all 3 fishes
		{
			if(fishlist[l].x<0)                                           //create new fish at new position if one cross the screen
			{
				fishlist[l].species=rand()%4;
				fishlist[l].x=800+rand()%1600;
				fishlist[l].y=150+rand()%400;
				fishlist[l].vx=fishlist[l].vy=0;
				fishlist[l].fishint=rand()%400;
			}
			else if(pl.harpfl && fishlist[l].x+fishsizex[fishlist[l].species]>=pl.harpx && fishlist[l].x<=pl.harpx && fishlist[l].y+fishsizey[fishlist[l].species]>=pl.harpy && fishlist[l].y<=pl.harpy)
			{                                                             //check if there was collision with harpoon
				pl.harpfl=0;
				fishlist[l].species=rand()%4;
				fishlist[l].x=800+rand()%1600;
				fishlist[l].y=rand()%400+150;
				fishlist[l].vx=fishlist[l].vy=0;
				score[2]++;                                               //increament score
				if(score[2]==58)
				{
					if(score[1]==' ') score[1]='1';
					else score[1]++;
					score[2]='0';
				}
				if(score[1]==58)
				{
					if(score[0]==' ') score[0]='1';
					else score[0]++;
					score[1]='0';
				}
			}
			else if(fishlist[l].x+fishsizex[fishlist[l].species]>=200 && fishlist[l].x<=264 && fishlist[l].y+fishsizey[fishlist[l].species]>=pl.ply && fishlist[l].y<=pl.ply+32)
			{                                                            //to see if fish attacking player
				pl.pllf-=10;                                             //decrease life
			}

			if(bx+64>=264 && bx<=264 && by+64>=pl.ply && by<=pl.ply && bubble) //check if player has got the bubble/air
			{
				pl.plair=100;                                            
				bubble=0;
			}

			if(fishlist[l].fishint>fishlist[l].x-200 && fishlist[l].x>200)    //decide whether fish will attack player
			{
				if((fishlist[l].y-pl.ply<0 && fishlist[l].y-pl.ply>-fishlist[l].fishint) || (fishlist[l].y-pl.ply>0 && fishlist[l].y-pl.ply<fishlist[l].fishint))
				{                                                             //
					if(pl.ply<fishlist[l].y && fishlist[l].vy>-10) fishlist[l].vy-=1;
					else if(fishlist[l].vy<10) fishlist[l].vy+=1;

					if(fishlist[l].vx>-10) fishlist[l].vx-=1;                
				}
				else
				{
					if(fishlist[l].vy) fishlist[l].vy-=.1*fishlist[l].vy;
					if(fishlist[l].vx>-10 ) fishlist[l].vx-=1;
				}
			}
			else
			{
				if(fishlist[l].vy) fishlist[l].vy-=.1*fishlist[l].vy;
				if(fishlist[l].vx>-10 ) fishlist[l].vx-=1;
			}

			fishlist[l].vx-=.1*fishlist[l].vx;                                //
			fishlist[l].vy-=.1*fishlist[l].vy;
			fishlist[l].x+=fishlist[l].vx;
			fishlist[l].y+=fishlist[l].vy;      //position change of fish
		}

		
		if(pl.harpfl)                           //harpoon existance
		{
			if(pl.harpx>800) pl.harpfl=0;       //if harpoon cross screen no harpoon
			pl.harpx+=25;                       //position change of harpoon
		}

		pl.plair-=.08;                          //air decrease
		pl.plvy-=.3*pl.plvy;                    //deacceleration of player to stop the player from moving while button isn't pressed
		if(pl.ply<562 && pl.ply>100)            //decision of player response according to position
		{
			pl.ply+=pl.plvy;
		}
		else if(pl.ply>=562 && pl.plvy<0)
		{
			pl.ply+=pl.plvy;
		}
		else if(pl.ply<=100 && pl.plvy>0)
		{
			pl.ply+=pl.plvy;
		}
		final=time(NULL);                      //getting time to change time-display & bubble existance 
		final1=time(NULL);
		if(final-ini>=1)                       //change of time string
		{
			ini=time(NULL);
            timestring[3]--;
			if(timestring[3]<48) 
			{
				timestring[3]=57;
				timestring[2]--;
			}
			if(timestring[2]==47)
			{
				timestring[2]=53;
				timestring[0]--;
			}
		}
		
		if(final1-ini1>30)                    //bubble creation control
		{
			ini1=time(NULL);
			bubble=1;
			bx=800;
			by=rand()%400+150;
		}

		if(bubble) bx-=5;                    //bubble position change
	}
}	


void iDraw()
{
	iClear();
	if(scrst==0)
	{
		iSetColor(0,255,0);
		iShowBMP(0,0,"fig/download1.bmp");
		iText(340,370,"Start",GLUT_BITMAP_HELVETICA_18);
		iText(340,320,"Best-score",GLUT_BITMAP_HELVETICA_18);
		iText(340,270,"Load game",GLUT_BITMAP_HELVETICA_18);
		iText(340,220,"Exit",GLUT_BITMAP_HELVETICA_18);
		iFilledPolygon(pointer[0],pointer[1],3);
	}
	else if(scrst==1)
	{
		iShowBMP(0,0,"fig/download1.bmp");
		iText(320,450,"Survive For 2 Minutes",GLUT_BITMAP_HELVETICA_18);
	}
	else if(scrst== 2)
	{
		int l;
		iShowBMP(0,0,"fig/download.bmp");
		for(l=0;l<3;l++)
		{
			iShowBMP(fishlist[l].x,fishlist[l].y,specieslist[fishlist[l].species]);
		}
		if(pl.harpfl)
		{
			iSetColor(0,0,0);
			iFilledRectangle(pl.harpx,pl.harpy,7,3);
		}
		
		if(picflag) iShowBMP(200,pl.ply,"fig/diver1.bmp");
		else iShowBMP(200,pl.ply,"fig/diver2.bmp");

		if(bubble) iShowBMP(bx,by,"fig/bubble.bmp");

		iSetColor(0,255,0);
		iFilledRectangle(20,560,pl.pllf,20);
		iSetColor(128,255,255);
		iFilledRectangle(20,520,pl.plair,20);
		iSetColor(255,0,0);
		iText(660,520,"Score:",GLUT_BITMAP_HELVETICA_18);
		iText(730,520,score,GLUT_BITMAP_HELVETICA_18);
		iText(660,550,timestring,GLUT_BITMAP_HELVETICA_18);
		iShowBMP(123,562,"fig/life.bmp");
		iShowBMP(123,522,"fig/air.bmp");
	}
	else if(scrst==3)
	{
		iShowBMP(0,0,"fig/download1.bmp");
		iSetColor(0,255,0);
		iText(340,370,"Game Over",GLUT_BITMAP_HELVETICA_18);
		if(pl.plair<0 || pl.pllf<0)
		{
			iText(340,345,"You Lose",GLUT_BITMAP_HELVETICA_18);
		}
		else
		{
			iText(340,345,"You Win",GLUT_BITMAP_HELVETICA_18);
		}
		iText(340,320,"SCORE:",GLUT_BITMAP_HELVETICA_18);
		iText(410,320,score,GLUT_BITMAP_HELVETICA_18);
	}
	else if(scrst==4)
	{
		iShowBMP(0,0,"fig/download.bmp");
		iSetColor(0,255,0);
		if(subst==0)
		{
			iText(340,370,"Save game",GLUT_BITMAP_HELVETICA_18);
			iText(340,320,"Load game",GLUT_BITMAP_HELVETICA_18);
			iText(340,270,"Exit",GLUT_BITMAP_HELVETICA_18);
		}
		else if(subst==1 || subst==2)
		{
			iText(340,400,"Saved game",GLUT_BITMAP_HELVETICA_18);
			iText(340,370,"1.",GLUT_BITMAP_HELVETICA_18);
			iText(353,370,save[0],GLUT_BITMAP_HELVETICA_18);
			iText(340,320,"2.",GLUT_BITMAP_HELVETICA_18);
			iText(353,320,save[1],GLUT_BITMAP_HELVETICA_18);
			iText(340,270,"3.",GLUT_BITMAP_HELVETICA_18);
			iText(353,270,save[2],GLUT_BITMAP_HELVETICA_18);
		}
		iFilledPolygon(pointer[0],pointer[1],3);
	}
	else if(scrst==5)
	{
		iShowBMP(0,0,"fig/download.bmp");
		iSetColor(0,255,0);
		iText(340,430,"New best-score:",GLUT_BITMAP_HELVETICA_18);
		iText(500,430,&bestscore[7],GLUT_BITMAP_HELVETICA_18);
		iText(340,400,"Enter Name:",GLUT_BITMAP_HELVETICA_18);
		iText(340,370,&bestscore[0],GLUT_BITMAP_HELVETICA_18);
	}
	else if(scrst==6)
	{
		iShowBMP(0,0,"fig/download.bmp");
		iSetColor(0,255,0);
		iText(340,400,"Best-score:",GLUT_BITMAP_HELVETICA_18);
		iText(340,370,bestscore,GLUT_BITMAP_HELVETICA_18);
	}	
}


void iMouseMove(int mx, int my)
{

}


void iMouse(int button, int state, int mx, int my)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
			
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
	
	}
}


void iKeyboard(unsigned char key)
{
	if(scrst==0)
	{
		if(key == '\r' && pointer[1][0]==365)
		{
			scrst=1;
			int l;
			for(l=0;l<3;l++)
			{
				fishlist[l].species=rand()%4;
				fishlist[l].x=rand()%1600+800;
				fishlist[l].y=rand()%400+150;
				fishlist[l].vx=0;
				fishlist[l].vy=0;
				fishlist[l].fishint=rand()%200;
			}
			pl.pllf=100;
			pl.plair=100;
			pl.ply=360;
			pl.plvy=0;
			pl.harpfl=0;
			score[0]=score[1]=' ';
			score[2]='0';
			score[3]='\0';
			timestring[2]=timestring[3]='0';
			timestring[0]='2';
			timestring[1]=':';
			timestring[4]='\0';
			iResumeTimer(t1);
		}
		else if(key == '\r' && pointer[1][0]==265)
		{
			scrst=4;
			subst=2;
			if((sg=fopen("save","rb+"))==NULL) sg=fopen("save","wb+");
			int l,l1;
			for(l=0;l<3;l++)
			{
				fseek(sg,l*(14+sizeof(fishlist)+sizeof(diver)),SEEK_SET);
				for(l1=0;l1<4;l1++) save[l][l1]=fgetc(sg);
				save[l][l1]='\0';
			}
			fseek(sg,0,SEEK_SET);
		}
		else if(key == '\r' && pointer[1][0]==215)
		{
			exit(0);
		}
		else if(key == '\r') 
		{
			scrst=6;
			if((sg=fopen("score","rb"))==NULL) sg=fopen("score","wb+");
			fread(bestscore,11,1,sg);
			fclose(sg);
		}
	}
	else if(scrst==2)
	{
		if(key == 'f' && !pl.harpfl)
		{
			pl.harpfl=1;
			pl.harpx=250;
			pl.harpy=pl.ply;
		}
		else if(key=='p')
		{
			iPauseTimer(t2);
			iPauseTimer(t3);
		}
		else if(key=='s')
		{
			iResumeTimer(t2);
			iResumeTimer(t3);
		}
		else if(key==27)
		{
			iPauseTimer(t2);
			iPauseTimer(t3);
			scrst=4;
		}
	}
	else if(scrst==3)
	{
		if(key=='\r') scrst=0;
	}
	else if(scrst==4)
	{
		if(key=='\r')
		{
			if(subst==0)
			{
				if(pointer[1][0]==365 || pointer[1][0]==315)
				{
					if(pointer[1][0]==365)
					{
						subst=1;
						chno=0;
					}
					else if(pointer[1][0]==315) subst=2;
					if((sg=fopen("save","rb+"))==NULL) sg=fopen("save","wb+");
					int l,l1;
					for(l=0;l<3;l++)
					{
						fseek(sg,l*(14+sizeof(fishlist)+sizeof(diver)),SEEK_SET);
						for(l1=0;l1<4;l1++) save[l][l1]=fgetc(sg);
						save[l][l1]='\0';
					}
					fseek(sg,0,SEEK_SET);
				}
				else
				{
					scrst=0;
					subst=0;
					pl.plair=-1;
				}
			}
			else if(subst==1)
			{
				if(pointer[1][0]==365) savegame(sg,0,0);
				else if(pointer[1][0]==315) savegame(sg,14+sizeof(fishlist)+sizeof(diver),1);
				else if(pointer[1][0]==265) savegame(sg,28+sizeof(fishlist)*2+sizeof(diver)*2,2);
				
				fclose(sg);
				chno=0;
				subst=0;
			}
			else if(subst==2)
			{
				int ofst;
				if(pointer[1][0]==365)  ofst=0;
				else if(pointer[1][0]==315)  ofst=14+sizeof(fishlist)+sizeof(diver);
				else if(pointer[1][0]==265)  ofst=28+sizeof(fishlist)*2+sizeof(diver)*2;

				if(loadgame(sg,ofst))
				{
					fclose(sg);
					subst=0;
					scrst=1;
					iResumeTimer(t1);
				}
				else
				{
					fclose(sg);
					subst=0;
					scrst=0;
				}
			}
		}
		else if(key==27)
		{
		    if(pl.plair>0)
			{
				subst=0;
				scrst=2;
				iResumeTimer(t2);
				iResumeTimer(t3);
			}
			else
			{
				scrst=0;
				subst=0;
				fclose(sg);
			}
		}
		else
		{
			if(scrst==4 && subst==1)
			{
				if(key!=8)
				{
					if(pointer[1][0]==365) save[0][chno]=key;
					else if(pointer[1][0]==315) save[1][chno]=key;
					else if(pointer[1][0]==265) save[2][chno]=key;
					if(chno<3)chno++;
				}
				else
				{
					if(pointer[1][0]==365) save[0][chno]=' ';
					else if(pointer[1][0]==315) save[1][chno]=' ';
					else if(pointer[1][0]==265) save[2][chno]=' ';
					if(chno) chno--;
				}
			}
		}
	}
	else if(scrst==5)
	{
		if(key=='\r')
		{
			bestscore[4]=' ';
			fseek(sg,0,SEEK_SET);
			fwrite(bestscore,11,1,sg);
			fclose(sg);
			chno=0;
			scrst=0;
		}
		else{
			if(key!=8)
			{
				bestscore[chno]=key;
				if(chno<3)chno++;
			}
			else
			{
				bestscore[chno]=' ';
				if(chno) chno--;
			}
		}
	}
	else if(scrst==6)
	{
		if(key=='\r' || key==27) scrst=0;
	}
}


void iSpecialKeyboard(unsigned char key)
{
	if(key == GLUT_KEY_END)
	{
		exit(0);	
	}
	if(scrst==0 || scrst==4)
	{
		if(key == GLUT_KEY_DOWN) pointermover(-1);
		
		else if(key == GLUT_KEY_UP) pointermover(1);	
	}
	else if(scrst==2)
	{
		if(key == GLUT_KEY_UP)  pl.plvy+=4;

		else if(key == GLUT_KEY_DOWN)  pl.plvy-=4;
	}
}

int main()
{
	t1=iSetTimer(2000,gameengine);
	iPauseTimer(t1);
	t2=iSetTimer(10,gameengine);
	iPauseTimer(t2);
	t3=iSetTimer(250,picflip);
	iPauseTimer(t3);
	pl.plair=-1;
	PlaySound("UnderWater Sounds.wav",NULL,SND_FILENAME | SND_ASYNC);
	iInitialize(800, 600, "UnderWater battle!");
	return 0;
}	
