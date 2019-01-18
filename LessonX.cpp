/////////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////
#include <Stdio.h>
#include "CommonAPI.h"
#include "LessonX.h"
#include "math.h"
////////////////////////////////////////////////////////////////////////////////
//
//
int			g_iGameState		=	1;		// ��Ϸ״̬��0 -- ��Ϸ�����ȴ���ʼ״̬��1 -- ���¿ո����ʼ����ʼ����Ϸ��2 -- ��Ϸ������
//
void		GameInit();
void		GameRun( float fDeltaTime );
void		GameEnd();
float fSpeedNorth = 0,fSpeedSouth = 0,fSpeedWest = 0,fSpeedEast = 0;
int DirectionStatus = 0;	//1--�� 2--���� 3--�� 4--���� 5--�� 6--���� 7--�� 8--���� 9--����ì�� 10--����ì��
int SkillDirection = 0;
char DirectionString[][50] = {"north", "eastnorth", "east", "eastsouth", "south", "westsouth", "west", "westnorth"};
char MonsterBehaviorString[][50]={"walking", "attack", "defend","die"};// 0--���� 1--���� 2--���� 3--����
char MonsterString[][50] = {"baiyuan","treeman","oldlady","dragon"};
char SkillString[][50] = {"fashubodong","zhuandong","jingu","utimateskill"};
int SpriteCounter = 0;
int SkillCounter = 0;
int MonsterCounter = 0;
int MonsterKill = 0;
int Counter = 0;
int Occupied = 0;
float SkillSpeedNorth;
float SkillSpeedSouth;
float SkillSpeedWest;
float SkillSpeedEast;
float ScreenLeft;
float ScreenTop;
float ScreenRight;
float ScreenBottom;
int CheckPoint = 0;// 1--��һ�� 2--�ڶ��� 3--������
int cnt = 0;
float SystemTimer;//ϵͳʱ��
float TimeRecorderStart;	//ϵͳ��ʱ��
float TimeRecorderEnd;
int isPlaying;
int BossHP;//boss��Ѫ��
int HpArray[4] = {4,4,3,4};
int DeHpArray[4] = {1,1,1,1};
int CheckPointFlag[4] = {0};
int MusicID;
typedef struct Skill{
	char* IdName;//���ܵ�������
	float posX;//Xλ��
	float posY;//Yλ��
	float VelocityX;//X�ٶ�
	float VelocityY;//Y�ٶ�
	int isVisible;// 1--�ɼ� 2--���ɼ�
	int DeHP;//���Լ���HP��ֵ
	int Direction; //����
	float LifeTime;//��λ��
	Skill* Next;
}skill,*pSkill;

typedef struct Monster{
	char* IdName;//�����������
	float HP;//Ѫ��
	float InitposX;//Xλ��
	float InitposY;//Yλ��
	char* birthpoint;
	float DesX;//Ŀ�ĵ�X
	float DesY;//Ŀ�ĵ�Y
	float VelocityX;//X�ٶ�
	float VelocityY;//Y�ٶ�
	int isVisible;// 1--�ɼ� 2--���ɼ�
	float LifeTime;//��λ��
	int MonsterFlag;// 0--��Գ 1--���� 2--���� 3--ħ��
	int Direction;//��������淽��
	float Radius;//���Χ
	int OutOfRadius; //��������
	float AnimationTime;//���ﶯ���Ĳ���ʱ��
	float TimeRecorderStart;//���ﶯ����ʱ��
	float TimeRecorderEnd;
	float TimeRecorderStart2;//���ﶯ����ʱ��
	float TimeRecorderEnd2;
	int IsPlaying;//��⶯���Ƿ����ڲ���
	int IsPlaying2;//��⶯���Ƿ����ڲ���
	int AdvancedBehavior;// 0--Ѳ�� 1--���� 2--���� 3--Ӧ����Ӧ
	Monster* Next;
}monster,*pMonster;

typedef struct Player{
	char* IdName;//�����������
	float HP;//Ѫ��
	float TimeRecorderStart;//�����ʱ��
	float TimeRecorderEnd;
	int IsPlaying;//��⶯���Ƿ����ڲ���
};

int Situation;	//0--���＼�ܴ򵽹��� 1--���＼�ܴ����� 2--���ܴ�����߽� 
pSkill SkillList = NULL;
pMonster MonsterList = NULL;
Player Foxer;

pSkill CreateSkill(int WhichSkill);
pMonster CreateMonster(int WhichMonster);
void AddSkill(pSkill SkillList,int WhichSkill);
void AddMonster(pMonster MonsterList, int WhichMonster);
void SetSkillInformation(pSkill New,int WhichSkill);
void SetMonsterInformation(pMonster New,int WhichMonster);
//void SearchSprite(pSkill SpriteList,const char* SkillName, const char* SpriteName);
void DeleteSkill(const char* SkillName);
void DeleteMonster(const char* MonsterName); 
void SpriteReleaseSkill(int iKey);
pSkill SearchSkill(const char* SkillName);
pMonster SearchMonster(const char* MonsterName);
void MonsterDie(const char* MonsterName);
void MonsterBehavior(const char* MonsterName,int MonsterStatus);//MonsterStatus�����������Ϊ
void AllMonsterStalk();//����ĸ߼���Ϊ������
void MonsterStalk(const char* MonsterName);
void MonsterPatrolling(const char* MonsterName);//����ĸ߼���Ϊ��Ѳ��
void MonsterFlee(const char* MonsterName);//����ĸ߼���Ϊ������
void MonsterReact(const char* MonsterName, const char* SkillName);//����ĸ߼���Ϊ��Ӧ����Ӧ
void UpdateInformation(const char* SkillName,const char* MonsterName);
float GetDistance(const char* ObjectOne,const char*ObjectTwo);
int OccupyBirthPoint();
void SceneFirst();
void SceneSecond();
void SceneFinal();
void Check();
//==============================================================================
//
// ����ĳ�������Ϊ��GameMainLoop����Ϊ��ѭ��������������ÿ֡ˢ����Ļͼ��֮�󣬶��ᱻ����һ�Ρ�


//==============================================================================
//
// ��Ϸ��ѭ�����˺���������ͣ�ĵ��ã�����ÿˢ��һ����Ļ���˺�����������һ��
// ���Դ�����Ϸ�Ŀ�ʼ�������С������ȸ���״̬. 
// ��������fDeltaTime : �ϴε��ñ��������˴ε��ñ�������ʱ��������λ����
void GameMainLoop( float	fDeltaTime )
{
	SystemTimer += fDeltaTime;//��ʱ
	switch( g_iGameState )
	{
		// ��ʼ����Ϸ�������һ���������
	case 1:
		{
			GameInit();
			g_iGameState	=	2; // ��ʼ��֮�󣬽���Ϸ״̬����Ϊ������
		}
		break;

		// ��Ϸ�����У����������Ϸ�߼�
	case 2:
		{
			// TODO �޸Ĵ˴���Ϸѭ�������������ȷ��Ϸ�߼�
			
			if( g_iGameState	=	2 )
			{
				GameRun( fDeltaTime );
			}
			else
			{
				// ��Ϸ������������Ϸ���㺯����������Ϸ״̬�޸�Ϊ����״̬
				g_iGameState	=	0;
				GameEnd();
			}
		}
		break;

		// ��Ϸ����/�ȴ����ո����ʼ
	case 0:
	default:
		break;
	};
}

//==============================================================================
//
// ÿ�ֿ�ʼǰ���г�ʼ���������һ���������
void GameInit()
{
	//SkillList = CreateSkill();
	
	ScreenLeft = dGetScreenLeft();
	ScreenTop = dGetScreenTop();
	ScreenRight = dGetScreenRight();
	ScreenBottom = dGetScreenBottom();
	dSetSpriteVisible("checkpoint",0);
	switch( CheckPoint )
	{
		case 0 :dLoadMap( "interface.t2d" );
				dStopSound(MusicID);
				MusicID = dPlaySound("firstM.ogg",1,1);
			break;
		case 1: 
			{
				
				dLoadMap( "level.t2d" );
				dStopSound(MusicID);
				MusicID = dPlaySound("firstC.ogg",1,1);
				dSetSpriteVisible("checkpoint",0);
				dSetSpriteVisible("baiyuan",1);
				Foxer.IdName = "foxer";
				Foxer.HP = 100;	//	����Ѫ��Ϊ10
				Occupied = 0;
				MonsterCounter == 0;
				for(int i = 0; i < 4; i++)
				{
					CheckPointFlag[i] = 0;
				}
				MonsterList = CreateMonster(3);
				for(int i = 0; i < 5; i++)
				{
					AddMonster(MonsterList,3);
				}	
			}
			break;
		case 2: 
			{
				//MonsterCounter == 0;
				dLoadMap( "second.t2d" );
				dStopSound(MusicID);
				MusicID = dPlaySound("scecondM.ogg",1,1);
				MonsterList = CreateMonster(2);
				TimeRecorderStart = SystemTimer;
				for(int i = 0; i < 5; i++)
				{
					AddMonster(MonsterList,2);
				}
				pMonster WorkPoint = MonsterList->Next;
				while(WorkPoint != NULL)
				{
					WorkPoint->AdvancedBehavior = 1; //����
					WorkPoint = WorkPoint->Next;
				}
			}
			break;
		case 3:
			{
				//MonsterCounter == 0;
				dLoadMap( "final.t2d" );
				//Foxer.HP = 1000;
				dStopSound(MusicID);
				MusicID = dPlaySound("finalM.ogg",1,1);
				BossHP = 100;
				MonsterKill = 0;
				char* BossName = "finalBoss";
				dSetSpriteCollisionReceive( "finalboss", 1 );
				dSetSpriteVisible("finalboss",1);
				dSetSpriteVisible("crystal",0);
			}
			break;
		case 6:
			{
				dLoadMap( "rule.t2d" );
			}
			break;
	}

}
//==============================================================================
//
// ÿ����Ϸ������
void GameRun( float fDeltaTime )
{
	switch( CheckPoint )
	{
		case 0:	
			break;
		case 1: SceneFirst();
			break;
		case 2:	SceneSecond();
			break;
		case 3: SceneFinal();
			break;
		case 4: dLoadMap( "victory.t2d" );
			break;
		case 5: dLoadMap( "failed.t2d" );
			break;
		case 6: 
			break;

	}
	
	//MonsterStalk();

}
//==============================================================================
//
// ������Ϸ����
void GameEnd()
{
	
}
//==========================================================================
//
// ����ƶ�
// ���� fMouseX, fMouseY��Ϊ��굱ǰ����
void OnMouseMove( const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// �����
// ���� iMouseType����갴��ֵ���� enum MouseTypes ����
// ���� fMouseX, fMouseY��Ϊ��굱ǰ����
void OnMouseClick( const int iMouseType, const float fMouseX, const float fMouseY )
{
	//dPlaySound("whoosh_mono.wav", 0, 1 );
	if(CheckPoint == 4 || CheckPoint == 5)
	{
		g_iGameState		=	1;
		CheckPoint = 0;
	}
	if( dIsPointInSprite( "play", fMouseX, fMouseY ) )
	{
		g_iGameState		=	1;
		CheckPoint = 1;
	}
	if( dIsPointInSprite( "rules", fMouseX, fMouseY ) )
	{
		g_iGameState		=	1;
		dStopSound(MusicID);
		MusicID = dPlaySound("rules.ogg",0,1);
		CheckPoint = 6;
	}
	if( dIsPointInSprite( "exit", fMouseX, fMouseY ) )
	{
		exit(0);
	}
	if( dIsPointInSprite( "back", fMouseX, fMouseY ) )
	{
		g_iGameState		=	1;
		CheckPoint = 0;
	}
}
//==========================================================================
//
// ��굯��
// ���� iMouseType����갴��ֵ���� enum MouseTypes ����
// ���� fMouseX, fMouseY��Ϊ��굱ǰ����
void OnMouseUp( const int iMouseType, const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// ���̰���
// ���� iKey�������µļ���ֵ�� enum KeyCodes �궨��
// ���� iAltPress, iShiftPress��iCtrlPress�������ϵĹ��ܼ�Alt��Ctrl��Shift��ǰ�Ƿ�Ҳ���ڰ���״̬(0δ���£�1����)
void OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress )
{	
	float SystemTime = 0,AnimationTime = 0;
	SystemTime = dGetTimeDelta();
	
	if( iKey == KEY_Q || iKey == KEY_W || iKey == KEY_E || iKey == KEY_R )
	{
		SpriteReleaseSkill(iKey);	
	}
	if(iKey == KEY_SPACE)
	{	
		dAnimateSpritePlayAnimation("foxer","foxskillxuli",1);
		AnimationTime = dGetAnimateSpriteAnimationTime("foxer");
		//SpriteReleaseSkill(iKey);	
	}
	if(iKey == KEY_T)
	{
		char* name[22];
		for(int i = 1; i<22;i++)
		{
			name[i] = dMakeSpriteName( "heart", i );
			dSetSpriteVisible(name[i],1);
		}
	}
	
	if( iKey == KEY_UP || iKey == KEY_DOWN || iKey == KEY_RIGHT || iKey == KEY_LEFT ) //ֻ����W S D A
	{
		//	dAnimateSpritePlayAnimation("birthpoint1","cure",0);
			SpriteMoveKeyDown(iKey);
	}
}
//==========================================================================
//
// ���̵���
// ���� iKey������ļ���ֵ�� enum KeyCodes �궨��
void OnKeyUp( const int iKey )
{
	if( iKey == KEY_UP || iKey == KEY_DOWN || iKey == KEY_RIGHT || iKey == KEY_LEFT )
	{
		SpriteMoveKeyUp(iKey);
	}
	if(iKey == KEY_T)
	{
		char* name[22];
		for(int i = 1; i<22;i++)
		{
			name[i] = dMakeSpriteName( "heart", i );
			dSetSpriteVisible(name[i],0);
		}
	}
}
//===========================================================================
//
// �����뾫����ײ
// ���� szSrcName��������ײ�ľ�������
// ���� szTarName������ײ�ľ�������
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	//if( strcmp(szSrcName,"foxer") == 0 && strstr(szTarName,"wall") != NULL)
	if( strcmp(szSrcName,"foxer") == 0)
	{
		dSetSpriteLinearVelocity("foxer",0,0);
	}
	if(strcmp(szSrcName,"foxer") != 0 && strstr(szTarName,"wall") == NULL)
	{
		Situation = 0;
	//dAnimateSpritePlayAnimation(szTarName,"bossattacksouth",1);
	//SearchSprite(SkillList, szSrcName, szTarName);
	
	dSetSpriteCollisionSend(szSrcName,0);
	dAnimateSpritePlayAnimation(szSrcName,"boom1",1);
	dPlaySound("kaipao.ogg",0,1);
	//dAnimateSpritePlayAnimation(szTarName,"foxskillxuli",1);
	dSetSpriteLinearVelocity(szTarName,0,0);
	UpdateInformation(szSrcName,szTarName);
	/*if(SearchSkill(szSrcName) != NULL);
	{
		DeleteSkill(szSrcName);
	}*/
	
	//DeleteMonster(szTarName);
	//MonsterBehavior(szTarName,0);
	}
}
//===========================================================================
//
// ����������߽���ײ
// ���� szName����ײ���߽�ľ�������
// ���� iColSide����ײ���ı߽� 0 ��ߣ�1 �ұߣ�2 �ϱߣ�3 �±�
void OnSpriteColWorldLimit( const char *szName, const int iColSide )
{

	if( SearchSkill(szName) != NULL )
	{
		Situation = 2;
		dAnimateSpritePlayAnimation(szName,"boom1",1);
		DeleteSkill(szName);
	}

}
void SpriteMoveKeyDown(int iKey)
{
	float Operator = 1;//�ٶȵ�ά������

	switch(iKey)	//	���������߼�
	{
		case KEY_UP:		
			fSpeedNorth = -15.f;
			SkillDirection = DirectionStatus = 1;//��
			dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
			break;
		case KEY_LEFT:
			fSpeedWest = -15.f;	
			SkillDirection = DirectionStatus = 7;//��
			dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0);
			break;
		case KEY_DOWN:	
			fSpeedSouth = 15.f;
			SkillDirection = DirectionStatus = 5;//��
			dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
			break;
		case KEY_RIGHT:
			fSpeedEast = 15.f;	
			SkillDirection = DirectionStatus = 3;//��
			dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);
			break;
	}			//���÷����ٶ� ������Ӧ���򶯻� ͬʱ�Ĵ淽��״̬
	
	if(fSpeedEast != 0 && fSpeedWest ==0)	
	{
		SkillDirection = DirectionStatus = 3;//��
		dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);	
	}
	if(fSpeedEast == 0 && fSpeedWest !=0)
	{
		SkillDirection = DirectionStatus = 7;//��
		dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0); 
	}
    if(fSpeedNorth == 0 && fSpeedSouth!=0)
	{
		SkillDirection = DirectionStatus = 5;//��
		dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
	}
	if(fSpeedNorth != 0 && fSpeedSouth ==0)
	{	
		SkillDirection = DirectionStatus = 1;//��
		dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
	}
	if(fSpeedEast != 0 && fSpeedWest !=0)
	{
		SkillDirection = DirectionStatus = 9;//����ì��
		if(iKey == KEY_RIGHT)
		{
			dAnimateSpritePlayAnimation("foxer","foxerstandingwest",0);
		}
		else dAnimateSpritePlayAnimation("foxer","foxerstandingeast",0);	//���Ȳ��ź��µİ���Ч��
	}	
	if(fSpeedNorth != 0 && fSpeedSouth !=0)
	{
		DirectionStatus = 10;//����ì��
		if(iKey == KEY_UP)
		{
			dAnimateSpritePlayAnimation("foxer","foxerstandingsouth",0);
		}
		else dAnimateSpritePlayAnimation("foxer","foxerstandingnorth",0);
	}
	if( fSpeedSouth > 0 && fSpeedEast > 0)
	{ 
		SkillDirection = DirectionStatus = 4;//����
		dAnimateSpritePlayAnimation("foxer","foxerwalkingeastsouth",0) ;
		Operator  = 1.414;//���Ŷ�
	}
	if( fSpeedSouth > 0 && fSpeedWest < 0)
	{
		SkillDirection = DirectionStatus = 6;//����
		dAnimateSpritePlayAnimation("foxer","foxerwalkingwestsouth",0);
		Operator  = 1.414;
	}
	if( fSpeedNorth < 0 && fSpeedWest < 0)
	{
		SkillDirection = DirectionStatus = 8;//����
		dAnimateSpritePlayAnimation("foxer","foxerwalkingwestnorth",0);
		Operator  = 1.414;
	}
	if( fSpeedNorth < 0 && fSpeedEast > 0)
	{
		SkillDirection = DirectionStatus = 2;//����
		dAnimateSpritePlayAnimation("foxer","foxerwalkingeastnorth",0);
		Operator  = 1.414;
	}

	dSetSpriteLinearVelocity("foxer", (fSpeedWest + fSpeedEast)/Operator, (fSpeedNorth + fSpeedSouth)/Operator);//�����ٶ� �ӷ���Ϊ��ͳһ���� ����Ϊ��
}
void SpriteMoveKeyUp(int iKey)
{
	switch(iKey) //�������� �ٶ�����
	{
		case KEY_UP:		
			fSpeedNorth = 0;
			//dAnimateSpritePlayAnimation("foxer","foxstandingnorth",0);
			break;
		case KEY_LEFT:
			fSpeedWest = 0;
			//dAnimateSpritePlayAnimation("foxer","foxstandingwest",0);
			break;
		case KEY_DOWN:	
			fSpeedSouth = 0;
			//dAnimateSpritePlayAnimation("foxer","foxstandingsouth",0);
			break;
		case KEY_RIGHT:
			fSpeedEast = 0;
			//dAnimateSpritePlayAnimation("foxer","foxstandingeast",0);
			break;
	}
	
	switch(DirectionStatus) //��ֹ����Ư�ƺ�����վ��״̬�ı������bug
	{
		case 1:
			if(fSpeedSouth !=0) //�������ߵ�ͬʱ�������ϵİ��� ���󲥷ž�ֹ���� ֮���ɿ������� ���������� �������ϵİ������Ѿ����� �����޷��ٽ���OnKeyDown �����������߶����޷����� ����������֮ǰ�ľ�ֹ���������ƶ� �γ�Ư��Bug
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
				SkillDirection = DirectionStatus = 5;
			}
			else {
				dAnimateSpritePlayAnimation("foxer","foxerstandingnorth",0);
			}
			break;
		case 2:
			if(fSpeedEast == 0 && fSpeedNorth == 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerstandingeastnorth",0);
			}
			else if(fSpeedEast != 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);
				SkillDirection = 3;
			}
			else {
				dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
				SkillDirection = 1;
			}
			break;
		case 3:
			if(fSpeedWest !=0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0);
				SkillDirection = DirectionStatus = 7;
			}
			else dAnimateSpritePlayAnimation("foxer","foxerstandingeast",0);
			break;
		case 4:
			if(fSpeedEast == 0 && fSpeedSouth == 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerstandingeastsouth",0);
			}
			else if(fSpeedEast != 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);
				SkillDirection = 3;
			}
			else {
				dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
				SkillDirection = 5;
			}
			break;
		case 5:
			if(fSpeedNorth !=0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
				SkillDirection = DirectionStatus = 1;
			}
			else dAnimateSpritePlayAnimation("foxer","foxerstandingsouth",0);
			break;
		case 6:
			if(fSpeedWest == 0 && fSpeedSouth == 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerstandingwestsouth",0);
			}
			else if(fSpeedWest != 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0);
				SkillDirection = 7;
			}
			else {
				dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
				SkillDirection = 5;
			}
			break;
		case 7:
			if(fSpeedEast !=0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);
				SkillDirection = DirectionStatus = 3;
			}
			else {
					dAnimateSpritePlayAnimation("foxer","foxerstandingwest",0);
			}
			break;
		case 8:
			if(fSpeedWest == 0 && fSpeedNorth == 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerstandingwestnorth",0);
			}
			else if(fSpeedWest != 0)
			{
				dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0);
				SkillDirection = 7;
			}
			else {
				dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
				SkillDirection = 1;
			}
			break;
		case 9:
			if(iKey == KEY_LEFT)
			{
				if( fSpeedEast != 0 )
				{
					dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);
					SkillDirection = DirectionStatus = 3;
				}
				else{
					dAnimateSpritePlayAnimation("foxer","foxerstandingwest",0);
				}
			}
			else if(iKey == KEY_RIGHT)
			{
				if( fSpeedWest != 0 )
				{
					dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0);
					SkillDirection = DirectionStatus = 7;
				}
				else
				dAnimateSpritePlayAnimation("foxer","foxerstandingeast",0);
			}
			break;
		case 10:
			if(iKey == KEY_UP)
			{
				if( fSpeedSouth != 0 )
				{
					dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
					SkillDirection = DirectionStatus = 5;
				}
				else
				{
					dAnimateSpritePlayAnimation("foxer","foxerstandingnorth",0);
				}
			}
			else if(iKey == KEY_DOWN)
			{
				if( fSpeedNorth != 0 )
				{
					dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
					SkillDirection = DirectionStatus = 1;
				}
				else
				dAnimateSpritePlayAnimation("foxer","foxerstandingsouth",0);
			}
			break;
	}
	dSetSpriteLinearVelocity("foxer", (fSpeedWest + fSpeedEast), (fSpeedNorth+fSpeedSouth));//�����ٶ�
}
void SpriteReleaseSkill(int iKey)
{
	int WhichSkill ;//����ѡ���жϱ���
	switch(iKey) //ѡ����
	{
	case KEY_Q:
		WhichSkill = 0;
		break;
	case KEY_W:
		WhichSkill = 1;
		break;
	case KEY_E:
		WhichSkill = 2;
		break;
	case KEY_R:
		WhichSkill = 3;
		break;
	}
	if(SkillCounter == 0)
	{
		SkillList = CreateSkill(WhichSkill);
	}
	else AddSkill(SkillList,WhichSkill);

	//while(WorkPoint != NULL)
	//{	
	//	/*if( SkillNumber < 5 )
	//	{
	//		SkillSize	=	8;
	//	}
	//	else if( SkillNumber >= 5 && SkillNumber < 10 )
	//	{
	//		SkillSize	=	10;
	//	}
	//	else
	//	{
	//		SkillSize	=	12;
	//	}*/
	//	WorkPoint->Name	=	dMakeSpriteName( "jinengmofa", SkillNumber );
	//	dCloneSprite( SkillTemplate[0], WorkPoint->Name );
	//	// ���һ��λ��
	//	iPosX	=	dRandomRange( dGetScreenLeft(), dGetScreenRight() );
	//	iPosY	=	dRandomRange( dGetScreenTop(), dGetScreenBottom() );
	//	dSetSpritePosition( WorkPoint->Name, (float)iPosX, (float)iPosY );

	//	WorkPoint = WorkPoint->Next;
	//	SkillNumber++;
	//}
}
pSkill CreateSkill(int WhichSkill)
{
	int skillNumber = 1;//���ܵĸ���

	pSkill Head = (pSkill)malloc(sizeof(Skill));//����һ���ڵ���ڴ�
	if (Head == NULL)
	{
		exit(-1);
	}
	pSkill Tail = Head;	//��������β�� ��ʼ��ʱ��ָ���׽ڵ�
	Tail->Next = NULL;	//���ڵ��ָ��ָ��Ϊ��

	for (int i = 0; i < skillNumber; i++)
	{
		pSkill New = (pSkill)malloc(sizeof(Skill));//�����½ڵ�
		if (New == NULL)
		{
			exit(-1);
		}
		SetSkillInformation(New,WhichSkill);//���뼼����Ϣ

		Tail->Next = New;//��ԭĩ�ڵ��βָ��ָ���½ڵ� �����ӽڵ�
		New->Next = NULL;//β�ڵ���ָ��ָ���
		Tail = New;//�½ڵ��Ϊβ�� ����ָ��Tail

	}
	SkillCounter++;
	return Head;//��������
}
void AddSkill(pSkill SkillList,int WhichSkill) 
{
	pSkill WorkPoint = SkillList;
	while (WorkPoint->Next != NULL)
	{
		WorkPoint = WorkPoint->Next;
	}	//�Ƚ�����ָ��ָ�����һ���ڵ�

	pSkill New = (pSkill)malloc( sizeof(Skill) );//����һ���µļ��ܽڵ�
	SetSkillInformation(New,WhichSkill);//���뼼����Ϣ
	New->Next = NULL;
	WorkPoint->Next = New;
	SkillCounter++;
}

void SetSkillInformation(pSkill New,int WhichSkill)
{
	float Operator = 1;		//ά������
	SkillSpeedNorth = SkillSpeedSouth = SkillSpeedEast = SkillSpeedWest = 0;//����ϸ�����ļ�¼
	//char Direction[50];		//����(������)���ַ���
	//char* SkillTemplate[4] = {"heartenergy","zhuandong","jingu","utimateskill"};
	New->posX =  dGetSpriteLinkPointPosX("foxer", 2 );		//���ü��ܵĳ�ʼX����
	New->posY =  dGetSpriteLinkPointPosY("foxer", 2 );		//���ü��ܵĳ�ʼY����
	New->DeHP = DeHpArray[WhichSkill];
	New->Direction = SkillDirection;	//��¼�ٶȵķ���
	//New->IdName	=	dMakeSpriteName( SkillTemplate[WhichSkill], SkillCounter );		//���輼�ܵ�IdName
	//dCloneSprite( SkillTemplate[WhichSkill], New->IdName );		//����ѡ���¡���� �������ܶ���
	New->IdName	=	dMakeSpriteName( SkillString[WhichSkill], SkillCounter );		//���輼�ܵ�IdName
	dCloneSprite( SkillString[WhichSkill], New->IdName );		//����ѡ���¡���� �������ܶ���

	dSetSpritePosition( New->IdName, New->posX, New->posY );		//���þ����λ��
	dSetSpriteWorldLimit(New->IdName, WORLD_LIMIT_NULL, ScreenLeft, ScreenTop, ScreenRight, ScreenBottom);//���ô���Ϊ���ܵ�����߽�
	dSetSpriteCollisionSend(New->IdName,true);

	if((fSpeedEast+fSpeedWest) == 0 &&(fSpeedNorth+fSpeedSouth) == 0)	//���þ�����ٶ�(ʸ��)
	{
		switch(SkillDirection)
		{
			case 1 : SkillSpeedNorth = -30;
				break;
			case 2 : SkillSpeedEast = 30;SkillSpeedNorth = -30;Operator = 1.414;
				break;
			case 3 : SkillSpeedEast = 30;
				break;
			case 4 : SkillSpeedEast = 30;SkillSpeedSouth = 30;Operator = 1.414;
				break;
			case 5 : SkillSpeedSouth = 30;
				break;
			case 6 : SkillSpeedWest = -30;SkillSpeedSouth = 30;Operator = 1.414;
				break;
			case 7 : SkillSpeedWest = -30;
				break;
			case 8 : SkillSpeedWest = -30;SkillSpeedNorth = -30;Operator = 1.414;
				break;
		}
		dSetSpriteLinearVelocity( New->IdName,( SkillSpeedWest + SkillSpeedEast )/Operator,( SkillSpeedNorth + SkillSpeedSouth )/Operator );
	}
	else {
		switch(SkillDirection)
		{
			case 1 : SkillSpeedNorth = -30;
				break;
			case 2 : SkillSpeedEast = 30;SkillSpeedNorth = -30;Operator = 1.414;
				break;
			case 3 : SkillSpeedEast = 30;
				break;
			case 4 : SkillSpeedEast = 30;SkillSpeedSouth = 30;Operator = 1.414;
				break;
			case 5 : SkillSpeedSouth = 30;
				break;
			case 6 : SkillSpeedWest = -30;SkillSpeedSouth = 30;Operator = 1.414;
				break;
			case 7 : SkillSpeedWest = -30;
				break;
			case 8 : SkillSpeedWest = -30;SkillSpeedNorth = -30;Operator = 1.414;
				break;
			/*case 1 : SkillSpeedNorth = fSpeedNorth;
				break;
			case 2 : SkillSpeedEast = fSpeedEast;SkillSpeedNorth = fSpeedNorth;Operator = 1.414;
				break;
			case 3 : SkillSpeedEast = fSpeedEast;
				break;
			case 4 : SkillSpeedEast = fSpeedEast;SkillSpeedSouth = fSpeedSouth;Operator = 1.414;
				break;
			case 5 : SkillSpeedSouth = 30;
				break;
			case 6 : SkillSpeedWest = fSpeedWest;SkillSpeedSouth = fSpeedSouth;Operator = 1.414;
				break;
			case 7 : SkillSpeedWest = fSpeedWest;
				break;
			case 8 : SkillSpeedWest = fSpeedWest;SkillSpeedNorth = fSpeedNorth;Operator = 1.414;
				break;*/
		}
		dSetSpriteLinearVelocity( New->IdName,( SkillSpeedWest + SkillSpeedEast )/Operator,( SkillSpeedNorth + SkillSpeedSouth )/Operator );
	}
	
	switch(WhichSkill)//�޸ļ���Ч��
	{
		case 0: 
			char AnimationString[50];	//���������� = �������� + ����
			strcpy(AnimationString,SkillString[WhichSkill]);	//���������ִ���������� �Ա����춯������ 
			strcat(AnimationString,DirectionString[SkillDirection-1]);	//д�뷽��
			//Direction = strcat(SkillString[WhichSkill],DirectionString[SkillDirection-1]);//ƴ�Ӽ�����(��������)
			dAnimateSpritePlayAnimation(New->IdName,AnimationString,0);//������Ӧ����
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		
	}
}
//void SetSkillInformation(pSkill New,int WhichSkill)
//{
//	float Operator = 1;		//ά������
//	SkillSpeedNorth = SkillSpeedSouth = SkillSpeedEast = SkillSpeedWest = 0;//����ϸ�����ļ�¼
//	char* Direction = NULL;		//����(������)���ַ���
//	//char* SkillTemplate[4] = {"heartenergy","zhuandong","jingu","utimateskill"};
//	New->posX =  dGetSpriteLinkPointPosX("foxer", 2 );		//���ü��ܵĳ�ʼX����
//	New->posY =  dGetSpriteLinkPointPosY("foxer", 2 );		//���ü��ܵĳ�ʼY����
//	New->DeHP = DeHpArray[WhichSkill];
//	//New->IdName	=	dMakeSpriteName( SkillTemplate[WhichSkill], SkillCounter );		//���輼�ܵ�IdName
//	//dCloneSprite( SkillTemplate[WhichSkill], New->IdName );		//����ѡ���¡���� �������ܶ���
//	New->IdName	=	dMakeSpriteName( SkillString[WhichSkill], SkillCounter );		//���輼�ܵ�IdName
//	dCloneSprite( SkillString[WhichSkill], New->IdName );		//����ѡ���¡���� �������ܶ���
//
//	dSetSpritePosition( New->IdName, New->posX, New->posY );		//���þ����λ��
//	dSetSpriteWorldLimit(New->IdName, WORLD_LIMIT_NULL, ScreenLeft, ScreenTop, ScreenRight, ScreenBottom);//���ô���Ϊ���ܵ�����߽�
//	dSetSpriteCollisionSend(New->IdName,true);
//
//	if((fSpeedEast+fSpeedWest) == 0 &&(fSpeedNorth+fSpeedSouth) == 0)	//���þ�����ٶ�(ʸ��)
//	{
//		switch(DirectionStatus)
//		{
//			case 1 : SkillSpeedNorth = -30;
//				break;
//			case 2 : SkillSpeedEast = 30;SkillSpeedNorth = -30;Operator = 1.414;
//				break;
//			case 3 : SkillSpeedEast = 30;
//				break;
//			case 4 : SkillSpeedEast = 30;SkillSpeedSouth = 30;Operator = 1.414;
//				break;
//			case 5 : SkillSpeedSouth = 30;
//				break;
//			case 6 : SkillSpeedWest = -30;SkillSpeedSouth = 30;Operator = 1.414;
//				break;
//			case 7 : SkillSpeedWest = -30;
//				break;
//			case 8 : SkillSpeedWest = -30;SkillSpeedNorth = -30;Operator = 1.414;
//				break;
//		}
//		dSetSpriteLinearVelocity( New->IdName,( SkillSpeedWest + SkillSpeedEast )/Operator,( SkillSpeedNorth + SkillSpeedSouth )/Operator );
//	}
//	else {
//		switch(DirectionStatus)
//		{
//			case 1 : SkillSpeedNorth = -30;
//				break;
//			case 2 : SkillSpeedEast = 30;SkillSpeedNorth = -30;Operator = 1.414;
//				break;
//			case 3 : SkillSpeedEast = 30;
//				break;
//			case 4 : SkillSpeedEast = 30;SkillSpeedSouth = 30;Operator = 1.414;
//				break;
//			case 5 : SkillSpeedSouth = 30;
//				break;
//			case 6 : SkillSpeedWest = -30;SkillSpeedSouth = 30;Operator = 1.414;
//				break;
//			case 7 : SkillSpeedWest = -30;
//				break;
//			case 8 : SkillSpeedWest = -30;SkillSpeedNorth = -30;Operator = 1.414;
//				break;
//			/*case 1 : SkillSpeedNorth = fSpeedNorth;
//				break;
//			case 2 : SkillSpeedEast = fSpeedEast;SkillSpeedNorth = fSpeedNorth;Operator = 1.414;
//				break;
//			case 3 : SkillSpeedEast = fSpeedEast;
//				break;
//			case 4 : SkillSpeedEast = fSpeedEast;SkillSpeedSouth = fSpeedSouth;Operator = 1.414;
//				break;
//			case 5 : SkillSpeedSouth = 30;
//				break;
//			case 6 : SkillSpeedWest = fSpeedWest;SkillSpeedSouth = fSpeedSouth;Operator = 1.414;
//				break;
//			case 7 : SkillSpeedWest = fSpeedWest;
//				break;
//			case 8 : SkillSpeedWest = fSpeedWest;SkillSpeedNorth = fSpeedNorth;Operator = 1.414;
//				break;*/
//		}
//		dSetSpriteLinearVelocity( New->IdName,( SkillSpeedWest + SkillSpeedEast )/Operator,( SkillSpeedNorth + SkillSpeedSouth )/Operator );
//	}
//	
//	switch(WhichSkill)//�޸ļ���Ч��
//	{
//		case 0: 
//			Direction = strcat(SkillString[WhichSkill],DirectionString[DirectionStatus-1]);//ƴ�Ӽ�����(��������)
//			dAnimateSpritePlayAnimation(New->IdName,Direction,0);//������Ӧ����
//			break;
//		case 1:
//			break;
//		case 2:
//			break;
//		case 3:
//			break;
//		
//	}
//}
//void DeleteSkill(pSkill ObjectPre ,int flag)// ɾ����һ����:flag == 0  ɾ�����о��飺flag ==1 
//{
//	SearchSkill();
//
//	pSkill ShipmentPoint = ObjectPre->Next;//Ŀ��ڵ� ���������ֶ�
//	char* SpriteName = ShipmentPoint->IdName;
//	switch (flag)
//	{
//	 case 0:
//	 {
//		 dDeleteSprite(SpriteName);
//		 ObjectPre->Next= ShipmentPoint->Next;//ǰ���ڵ��βָ��ָ��ο��ڵ��ڵ�
//		 free(ShipmentPoint); //�ͷŲο��ڵ�Ŀռ�
//		 ShipmentPoint = NULL; //��ֹ������ָ��
//		 if(Counter%2 == 0)
//		 {dSetSpriteLinearVelocity("jijione",20,0);}
//		 else
//		{dSetSpriteLinearVelocity("jijione",-20,0);}
//		 Counter++;
//	 }
//		 break;
//	 case1:
//		 while (ObjectPre != NULL) {
//			 ShipmentPoint = ObjectPre;        //Shipmentָ��Ҫɾ���Ľڵ�
//			 ObjectPre = ObjectPre->Next;//�ƶ�����һ���ڵ�
//			 free(ShipmentPoint);    //ɾ��ԭ�ڵ�
//		 }
//		 break;
//	}
//}
void DeleteSkill(const char* SkillName)	
{
	pSkill SkillPoint= SearchSkill(SkillName);	//�ҵ����ܵĽڵ� 
	pSkill ObjectPre = NULL;	//���ܵ�ǰ���ڵ�
	pSkill ShipmentPoint = SkillPoint;	//��ת�ڵ� ���������ֶ�
	ObjectPre = SkillList;	//׼����������
	while( ObjectPre->Next != SkillPoint)
	{
		ObjectPre = ObjectPre->Next;
	}
	dDeleteSprite(SkillPoint->IdName);	//��ͼ��ɾ������
	ObjectPre->Next= ShipmentPoint->Next;	//ǰ���ڵ��βָ��ָ��ο��ڵ��ڵ�
	free(ShipmentPoint);	//�ͷŲο��ڵ�Ŀռ�
	ShipmentPoint = NULL;	//��ֹ������ָ��

	if(Counter%2 == 0)
	{
		dSetSpriteLinearVelocity("jijione",20,0);
	}
	else
	{
		dSetSpriteLinearVelocity("jijione",-20,0);
	}
	Counter++;
}
void DeleteMonster(const char* MonsterName)	
{
	pMonster MonsterPoint= SearchMonster(MonsterName);	//�ҵ�����Ľڵ� 
	pMonster ObjectPre = NULL;	//�����ǰ���ڵ�
	pMonster ShipmentPoint = MonsterPoint;	//��ת�ڵ� ���������ֶ�
	ObjectPre = MonsterList;	//׼����������
	while( ObjectPre->Next != MonsterPoint)
	{
		ObjectPre = ObjectPre->Next;
	}
	dDeleteSprite(MonsterPoint->IdName);	//��ͼ��ɾ������
	ObjectPre->Next= ShipmentPoint->Next;	//ǰ���ڵ��βָ��ָ��ο��ڵ��ڵ�
	free(ShipmentPoint);	//�ͷŲο��ڵ�Ŀռ�
	ShipmentPoint = NULL;	//��ֹ������ָ��

	if(Counter%2 == 0)
	{
		dSetSpriteLinearVelocity("jijione",20,0);
	}
	else
	{
		dSetSpriteLinearVelocity("jijione",-20,0);
	}
	Counter++;
}
	
pSkill SearchSkill(const char* SkillName)
{
	if(SkillList == NULL)
	{
		return NULL;	//δ����ָ���б�
	}
	pSkill SkillSearchPoint = SkillList->Next;
	int flag = 0;
	while (SkillSearchPoint != NULL && flag != 1)
	{
		if(strcmp(SkillSearchPoint->IdName ,SkillName) == 0)
		{
			flag = 1;
		}
		else
		{
			SkillSearchPoint = SkillSearchPoint->Next;	//δ�ҵ��������һ���ڵ�
		}
	}
	if(flag)
	{
		return SkillSearchPoint;
	}
	else return NULL;
}
pMonster SearchMonster(const char* MonsterName)
{
	pMonster MonsterSearchPoint;

	if(MonsterList != NULL)
	{
		MonsterSearchPoint = MonsterList->Next;
	}
	else return NULL;

	int flag = 0;
	while (MonsterSearchPoint != NULL && flag != 1)
	{
		if(strcmp(MonsterSearchPoint->IdName ,MonsterName) == 0)
		{
			flag = 1;
		}
		else
		{
			MonsterSearchPoint = MonsterSearchPoint->Next;	//δ�ҵ��������һ���ڵ�
		}
	}
	if(flag)
	{
		return MonsterSearchPoint;
	}
	else return NULL;
}
//void SearchSprite(pSkill SkillList, const char* SkillName, const char* SpriteName)
//{
//	//dAnimateSpritePlayAnimation(SpriteName,"yuanqidan2Animation",1);
//	pSkill SkillSearchPoint = SkillList->Next;
//	pMonster MonsterSearchPoint = NULL;
//	int flagOne = 0,flagTwo = 0;
//	while (SkillSearchPoint != NULL && flagOne != 1)
//	{
//		if(strcmp(SkillSearchPoint->IdName ,SkillName) == 0)
//		{
//			flagOne = 1;
//		}
//		else
//		{
//			SkillSearchPoint = SkillSearchPoint->Next;	//δ�ҵ��������һ���ڵ�
//		}
//	}
//	while (MonsterSearchPoint != NULL && flagTwo != 1)
//	{
//		if(strcmp(MonsterSearchPoint->IdName ,SpriteName) == 0)
//		{
//			flagTwo = 1;
//		}
//		else
//		{
//			MonsterSearchPoint = MonsterSearchPoint->Next;	//δ�ҵ��������һ���ڵ�
//		}
//	}
//	switch(Situation)
//	{
//		case 0:{	//���＼�ܴ򵽹���
//				//
//		   }
//			break;
//		case 1:{	//���＼�ܴ�����
//				
//			}
//			break;
//		case 2:{	//���ܴ�����߽�
//					pSkill ObjectPre = SkillList;
//					while (ObjectPre->Next != SkillSearchPoint)
//					{
//						ObjectPre = ObjectPre->Next;
//					}	//Ѱ��Ŀ��ڵ��ǰ���ڵ�
//					DeleteSprite(ObjectPre,0);	
//		}
//			break;		
//	}
//				
//}
pMonster CreateMonster(int WhichMonster)
{
	int MonsterNumber = 1;//����ĸ���

	pMonster Head = (pMonster)malloc(sizeof(Monster));//����һ���ڵ���ڴ�
	if (Head == NULL)
	{
		exit(-1);
	}
	pMonster Tail = Head;	//��������β�� ��ʼ��ʱ��ָ���׽ڵ�
	Tail->Next = NULL;	//���ڵ��ָ��ָ��Ϊ��

	for (int i = 0; i < MonsterNumber; i++)
	{
		pMonster New = (pMonster)malloc(sizeof(Monster));//�����½ڵ�
		if (New == NULL)
		{
			exit(-1);
		}
		SetMonsterInformation(New,WhichMonster);//���������Ϣ

		Tail->Next = New;//��ԭĩ�ڵ��βָ��ָ���½ڵ� �����ӽڵ�
		New->Next = NULL;//β�ڵ���ָ��ָ���
		Tail = New;//�½ڵ��Ϊβ�� ����ָ��Tail

	}
	MonsterCounter++;
	return Head;//��������
}
void AddMonster(pMonster MonsterList,int WhichMonster) 
{
	pMonster WorkPoint = MonsterList;
	while (WorkPoint->Next != NULL)
	{
		WorkPoint = WorkPoint->Next;
	}	//�Ƚ�����ָ��ָ�����һ���ڵ�

	pMonster New = (pMonster)malloc( sizeof(Monster) );//����һ���µļ��ܽڵ�
	SetMonsterInformation(New,WhichMonster);//���뼼����Ϣ
	New->Next = NULL;
	WorkPoint->Next = New;
	MonsterCounter++;
}
void SetMonsterInformation(pMonster New, int WhichMonster)
{
	//New->InitposX =  dRandomRange(ScreenLeft, ScreenRight);		//���ù���ĳ�ʼX����
	//New->InitposY =  dRandomRange(ScreenTop, ScreenBottom);		//���ù���ĳ�ʼY����
	char* birthpointName = dMakeSpriteName("birthpoint",(int)dRandomRange(1,4));
	New->birthpoint = birthpointName;	//��¼�����������
	New->InitposX =  dGetSpritePositionX(birthpointName);		//���ù���ĳ�ʼX����
	New->InitposY =  dGetSpritePositionY(birthpointName)-3;		//���ù���ĳ�ʼY����
	New->IdName	=	dMakeSpriteName( MonsterString[WhichMonster], MonsterCounter );		//��������IdName
	New->IsPlaying = 0;	//δ�ڲ��Ų�ѭ������
	New->MonsterFlag = WhichMonster;	//����ĸ�����
	New->HP = HpArray[WhichMonster];	//�����Ѫ��ֵ
	New->Radius = 4;	//���������뾶
	New->OutOfRadius = 0;	//��������
	New->AdvancedBehavior = 0;//Ѳ��
	New->TimeRecorderEnd = 0;//�����ʱ����ʼ��
	New->TimeRecorderStart = 0;
	dCloneSprite( MonsterString[WhichMonster], New->IdName );		//����ѡ���¡���� �������ܶ���
	dSetSpritePosition( New->IdName, New->InitposX, New->InitposY );		//���þ����λ��
	dSetSpriteWorldLimit(New->IdName, WORLD_LIMIT_NULL, ScreenLeft, ScreenTop, ScreenRight, ScreenBottom);		//���ô���Ϊ���ܵ�����߽�
	dSetSpriteCollisionReceive(New->IdName,true);		//������ײ
}
void MonsterDie(const char* MonsterName)
{
	//������������
	DeleteMonster(MonsterName);//ɾ�����������Ϣ
}
void Check()
{
	//float posX = 0,posY = 0;
	////int IsPlaying = 0;//���ڲ���
	//posX = dGetSpritePositionX("foxer");
	//posY = dGetSpritePositionY("foxer");
	//pMonster WorkPoint = MonsterList->Next;

	////if(cnt == 0 )
	////{
	////	MonsterBehavior(WorkPoint->IdName,0);
	////}
	//while(WorkPoint != NULL)
	//{	
	//	float DeltaX = posX - dGetSpritePositionX(WorkPoint->IdName);	//����������X����Ĳ�ֵ
	//	float DeltaY = posY - dGetSpritePositionY(WorkPoint->IdName);	//����������Y����Ĳ�ֵ
	//	if(	DeltaY < 3 && DeltaY > -3 && DeltaX < 3 && DeltaX > -3 && WorkPoint->IsPlaying == 0 )	//�жϹ����Ƿ����������Ϊ���ĵ�3*3�������� �����򲥷Ŷ���
	//	{
	//		/* �����������ķ������򴥷��������� */
	//		dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//�����ٶ�����
	//		dAnimateSpritePlayAnimation(WorkPoint->IdName,"foxerattack",1);		//���Ź�������
	//		TimeRecorderStart = SystemTimer;	//��ʱ��ʼ
	//		WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//�������ڲ��Ŷ�����ʱ��
	//		//IsPlaying = 1;
	//		WorkPoint->IsPlaying = 1;
	//	}
	//	else if(WorkPoint->IsPlaying == 0)// �����Ź��������Ҳ���������
	//	{
	//		MonsterBehavior(WorkPoint->IdName,0);		//ȷ�ϸ��ٷ���
	//		dSpriteMoveTo(WorkPoint->IdName,posX,posY,12,1);		//����
	//	}
	//	TimeRecorderEnd = SystemTimer;
	//	if( (TimeRecorderEnd-TimeRecorderStart) >= 1.5)		//ϵͳʱ��Ĳ�ֵ���ڶ���ʱ���򲥷����
	//	{
	//		//IsPlaying = 0;
	//		WorkPoint->IsPlaying = 0;
	//	}
	//		
	//	//if(DeltaY < 3 && DeltaY > -3 && DeltaX < 3 && DeltaX > -3 && IsPlaying == 0)		//�жϹ����Ƿ����������Ϊ���ĵ�3*3��������
	//	//{
	//	//	TimeRecorderStart = SystemTimer;	//��ʱ��ʼ
	//	//	dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//�����ٶ�����
	//	//	dAnimateSpritePlayAnimation(WorkPoint->IdName,"foxerattack",1);		//���Ź�������
	//	//	IsPlaying = 1;	//���ڲ��Ŷ����ı�־
	//	//	WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//�������ڲ��Ŷ�����ʱ��
	//	//
	//	//	if(Counter%2 == 0)
	//	//	{
	//	//		dSetSpriteLinearVelocity("jijione",20,0);
	//	//	}
	//	//	else
	//	//	{
	//	//		dSetSpriteLinearVelocity("jijione",-20,0);
	//	//	}
	//	//	Counter++;
	//	//}
	//	
	//	WorkPoint = WorkPoint->Next;
	//}
	//cnt++;
	int	isDone = 0;
	isDone = OccupyBirthPoint();
	if(OccupyBirthPoint() && CheckPoint == 1)	//ռ������� 
	{
		dSetSpriteVisible("baiyuan",0);		//boss��ʧ
		dSetSpriteVisible("checkpoint",1);		//checkpoint����
	}
	
	if( GetDistance("foxer","checkpoint") <= 3 && isDone == 1)	//ͨ��
	{
			if(CheckPoint==2)
			{
				CheckPoint = 3;
			}
			if( CheckPoint ==1)
			{
				CheckPoint = 2;
			}
		
		g_iGameState = 1;
	}
	else
	{	pMonster WorkPoint = MonsterList->Next;
		while(WorkPoint != NULL)
		{
			switch(WorkPoint->AdvancedBehavior)
			{
			case 0: MonsterPatrolling(WorkPoint->IdName);
				break;
			case 1: MonsterStalk(WorkPoint->IdName);
				break;
			case 2: MonsterFlee(WorkPoint->IdName);
				break;
			case 3: MonsterReact(WorkPoint->IdName,NULL);
				break;
			}
		

			WorkPoint = WorkPoint->Next;
		}
	}
}
void MonsterReact(const char* MonsterName,const char* SkillName)
{
	pMonster MonsterPoint = SearchMonster(MonsterName);		//����ڵ�
	pSkill SkillPoint;

	if(SkillName !=NULL)
	{
		SkillPoint = SearchSkill(SkillName);		//�ҵ����ܽڵ��Ի�ȡ���ܵķ���
		if( MonsterPoint->IsPlaying2 != 0 )	//���˵�ʱ��
		{
			int SkillDiretion = SkillPoint->Direction;		//��ȡ���ܷ���
			int RetreatDiretion = 0;	//������˷���
			float MonsterSpeedWest = 0 ,MonsterSpeedEast = 0,MonsterSpeedNorth = 0,MonsterSpeedSouth = 0;
			if(SkillDiretion < 5)
			{
				RetreatDiretion = SkillDiretion + 4;
			}
			else
			{
				RetreatDiretion = SkillDiretion - 4;
			}
			int Operator = 1;
			switch(SkillDirection)
				{
					case 1 : MonsterSpeedNorth = -20;
						break;
					case 2 : MonsterSpeedEast = 20;MonsterSpeedNorth = -20;Operator = 1.414;
						break;
					case 3 : MonsterSpeedEast = 20;
						break;
					case 4 : MonsterSpeedEast = 20;MonsterSpeedSouth = 20;Operator = 1.414;
						break;
					case 5 : MonsterSpeedSouth = 20;
						break;
					case 6 : MonsterSpeedWest = -20;MonsterSpeedSouth = 20;Operator = 1.414;
						break;
					case 7 : MonsterSpeedWest = -20;
						break;
					case 8 : MonsterSpeedWest = -20;MonsterSpeedNorth = -20;Operator = 1.414;
						break;
				}
			dSetSpriteLinearVelocity( MonsterName,( MonsterSpeedWest + MonsterSpeedEast )/Operator,( MonsterSpeedNorth + MonsterSpeedSouth )/Operator );
			MonsterPoint->IsPlaying2 = 1;	//��ʼ������
			MonsterPoint->TimeRecorderStart2 = SystemTimer;	//��ʼ��ʱ

		}
	}
	MonsterPoint->TimeRecorderEnd2 = SystemTimer;
	if( (MonsterPoint->TimeRecorderEnd2 - MonsterPoint->TimeRecorderStart2) > 0.3)
	{
		MonsterPoint->AdvancedBehavior = 1;
		MonsterPoint->IsPlaying = 0;
		MonsterPoint->TimeRecorderEnd2 = MonsterPoint->TimeRecorderStart2 = 0;
		dSetSpriteLinearVelocity( MonsterName,0,0 );
	}
}
void MonsterFlee(const char* MonsterName)
{
	pMonster MonsterPoint = NULL;	
	MonsterPoint = SearchMonster(MonsterName);	//�ҵ������й������ڽڵ�

	/*ͨ��λ�ò�ֵΪ���� ����Ϊ����ԭ�� ���γɵ�ƽ��ֱ������ϵ ȷ������ĳ���*/
	float BirthPointX = 0,BirthPointY = 0,FollowerX = 0,FollowerY = 0;
	float DeltaX = 0,DeltaY = 0;
	int DirectionStatusM;//���﷽��״̬
	char* BirthPoint;	//�����������
	BirthPoint = MonsterPoint->birthpoint;	//�����������
	BirthPointX = MonsterPoint->InitposX;		//������λ����Ϣ
	BirthPointY = MonsterPoint->InitposY;	
	FollowerX = dGetSpritePositionX(MonsterName);	//����λ����Ϣ
	FollowerY = dGetSpritePositionY(MonsterName);
	DeltaX = BirthPointX - FollowerX;		//���ߵ�λ�ò�ֵ(������Ϊ��������)
	DeltaY = BirthPointY - FollowerY;
	float Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );
	if( Distance <= 2 )
	{
		MonsterPoint->HP = HpArray[MonsterPoint->MonsterFlag];	//�ظ���Ѫ
		//dAnimateSpritePlayAnimation(BirthPoint,"cure",1);	//������������
		MonsterPoint->AdvancedBehavior = 1;	//�ظ�������Ϊ
	}
	else 
	{
		if(DeltaX > 3 && DeltaY > 3)
		{
			DirectionStatusM = 4;
		}
		if(DeltaX > 3 && DeltaY < -3)
		{
			DirectionStatusM = 2;
		}
		if(DeltaX < -3 && DeltaY > 3)
		{
			DirectionStatusM = 6;
		}
		if(DeltaX < -3 && DeltaY < -3)
		{
			DirectionStatusM = 8;
		}
		if(DeltaX < 3 && DeltaX > -3 && DeltaY > 0)
		{
			DirectionStatusM = 5;
		}
		if( DeltaX < 3 && DeltaX > -3 && DeltaY < 0)
		{
			DirectionStatusM = 1;
		}
		if( DeltaY < 3 && DeltaY > -3 && DeltaX > 0)
		{
			DirectionStatusM = 3;
		}
		if( DeltaY < 3 && DeltaY > -3 && DeltaX < 0)
		{
			DirectionStatusM = 7;
		}
		char AnimationString[50];	//���������� = ����ģ������ + ������Ϊ +���﷽��(����)
		strcpy(AnimationString,MonsterString[MonsterPoint->MonsterFlag]);	//���������ִ���������� �Ա����춯������ �����ﲻ����IdName)
		strcat(AnimationString,MonsterBehaviorString[0]);	//����
		strcat(AnimationString,DirectionString[DirectionStatusM - 1]);	//д�뷽��
		if(MonsterPoint->Direction != DirectionStatusM){	//ͨ����¼������Բ�׽����ĸı� ���⶯�����ظ�����
			MonsterPoint->Direction = DirectionStatusM;
			dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//���Ŷ���
		}
		dSpriteMoveTo(MonsterName,MonsterPoint->InitposX,MonsterPoint->InitposY,15,1);
	}
}
void AllMonsterStalk()
{
	float posX = 0,posY = 0;
	posX = dGetSpritePositionX("foxer");
	posY = dGetSpritePositionY("foxer");
	pMonster WorkPoint = MonsterList->Next;

	while(WorkPoint != NULL)
	{	
		WorkPoint->AdvancedBehavior = 1;
		float DeltaX = posX - dGetSpritePositionX(WorkPoint->IdName);	//����������X����Ĳ�ֵ
		float DeltaY = posY - dGetSpritePositionY(WorkPoint->IdName);	//����������Y����Ĳ�ֵ
		float Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );//�������˵ľ���
		if(	Distance <= 3 && WorkPoint->IsPlaying == 0 )	//�жϹ����Ƿ����������Ϊ���ĵ�3*3�������� �����򲥷Ŷ���
		{
			/* �����������ķ������򴥷��������� */
			dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//�����ٶ�����
			char AnimationString[70];	//���������� = ����ģ������ + ������Ϊ +���﷽��(����)
			strcpy(AnimationString,MonsterString[WorkPoint->MonsterFlag]);	//���������ִ���������� �Ա����춯������ �����ﲻ����IdName)
			strcat(AnimationString,MonsterBehaviorString[1]);	//��������Ϊд��(���ⲿ��������)
			strcat(AnimationString,DirectionString[WorkPoint->Direction - 1]);	//д�뷽��
			dAnimateSpritePlayAnimation(WorkPoint->IdName,AnimationString,1);		//���Ź�������
			//dAnimateSpritePlayAnimation(WorkPoint->IdName,"foxerattack",1);		//���Ź�������
			WorkPoint->TimeRecorderStart = SystemTimer;	//��ʱ��ʼ
			WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//�������ڲ��Ŷ�����ʱ��
			WorkPoint->IsPlaying = 1;
			


		}
		else if(WorkPoint->IsPlaying == 0)// �����Ź��������Ҳ���������
		{
			MonsterBehavior(WorkPoint->IdName,0);		//ȷ�ϸ��ٷ���
			dSpriteMoveTo(WorkPoint->IdName,posX,posY,12,1);		//����
		}
		WorkPoint->TimeRecorderEnd = SystemTimer;
		if( (WorkPoint->TimeRecorderEnd-WorkPoint->TimeRecorderStart) >= 1.5)		//ϵͳʱ��Ĳ�ֵ���ڶ���ʱ���򲥷����
		{
			WorkPoint->IsPlaying = 0;
			/*��չ����ʱ��������*/
			WorkPoint->TimeRecorderStart = 0;
			WorkPoint->TimeRecorderEnd = 0;
		}
		WorkPoint = WorkPoint->Next;
	}
	
}
void MonsterStalk(const char* MonsterName)
{
	float posX = 0,posY = 0;
	posX = dGetSpritePositionX("foxer");
	posY = dGetSpritePositionY("foxer");
	pMonster WorkPoint = SearchMonster(MonsterName);

		//WorkPoint->AdvancedBehavior = 1;
		float DeltaX = posX - dGetSpritePositionX(WorkPoint->IdName);	//����������X����Ĳ�ֵ
		float DeltaY = posY - dGetSpritePositionY(WorkPoint->IdName);	//����������Y����Ĳ�ֵ
		float Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );//�������˵ľ���

		//if(	DeltaY < 3 && DeltaY > -3 && DeltaX < 3 && DeltaX > -3 && WorkPoint->IsPlaying == 0 )	//�жϹ����Ƿ����������Ϊ���ĵ�3*3�������� �����򲥷Ŷ���
		if(	Distance <= 3 && WorkPoint->IsPlaying == 0 )	//�жϹ����Ƿ����������Ϊ���ĵ�3*3�������� �����򲥷Ŷ���
		{
			/* �����������ķ������򴥷��������� */
			dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//�����ٶ�����

			char AnimationString[70];	//���������� = ����ģ������ + ������Ϊ +���﷽��(����)
			strcpy(AnimationString,MonsterString[WorkPoint->MonsterFlag]);	//���������ִ���������� �Ա����춯������ �����ﲻ����IdName)
			strcat(AnimationString,MonsterBehaviorString[1]);	//��������Ϊд��(���ⲿ��������)
			strcat(AnimationString,DirectionString[WorkPoint->Direction - 1]);	//д�뷽��
			dAnimateSpritePlayAnimation(WorkPoint->IdName,AnimationString,1);		//���Ź�������
			
			Foxer.HP -= 3;//��Ѫ
			if( Foxer.HP <= 0)
			{
				CheckPoint = 5;
			}
			WorkPoint->TimeRecorderStart = SystemTimer;	//��ʱ��ʼ
			WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//�������ڲ��Ŷ�����ʱ��
			WorkPoint->IsPlaying = 1;
		}
		else if(WorkPoint->IsPlaying == 0)// �����Ź��������Ҳ���������
		{
			MonsterBehavior(WorkPoint->IdName,0);		//ȷ�ϸ��ٷ���
			if(WorkPoint->AdvancedBehavior == 1)
			{
				dSpriteMoveTo(WorkPoint->IdName,posX,posY,12,1);		//����
			}
		}
		WorkPoint->TimeRecorderEnd = SystemTimer;
		if( (WorkPoint->TimeRecorderEnd-WorkPoint->TimeRecorderStart) >= 1.5)		//ϵͳʱ��Ĳ�ֵ���ڶ���ʱ���򲥷����
		{
			WorkPoint->IsPlaying = 0;
			/*��չ����ʱ��������*/
			WorkPoint->TimeRecorderStart = 0;
			WorkPoint->TimeRecorderEnd = 0;
		}
	
}
void MonsterPatrolling(const char* MonsterName)
{
		pMonster MonsterPoint = SearchMonster(MonsterName);
		MonsterPoint->TimeRecorderEnd = SystemTimer;//��ʱ������
		int DirectionStatusM;//���﷽��״̬
		float RealPosX = 0,RealPosY = 0;	//���Ｔʱ��λ��
		float InitPosX = 0,InitPosY = 0;	//���������λ��
		float PlayerPosX = 0, PlayerPosY = 0;	//��ҵ�λ��
		RealPosX = dGetSpritePositionX(MonsterName);
		RealPosY = dGetSpritePositionY(MonsterName);
		InitPosX = MonsterPoint->InitposX;
		InitPosY = MonsterPoint->InitposY;
		PlayerPosX = dGetSpritePositionX("foxer");
		PlayerPosY = dGetSpritePositionY("foxer");

		float DeltaX = PlayerPosX-RealPosX;	//����������X����Ĳ�ֵ
		float DeltaY = PlayerPosY - RealPosY;	//����������Y����Ĳ�ֵ
		float DistanceOfPlayer = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );//�������˵ľ���
		float Radius = MonsterPoint->Radius;	//����뾶
		float DistanceOfBirthpoint = 0;	//������������ľ���
		DistanceOfBirthpoint = sqrt( (RealPosX - InitPosX)*(RealPosX - InitPosX) + (RealPosY - InitPosY)*(RealPosY - InitPosY) );	//������㹫ʽ
		
		if( DistanceOfPlayer <= 10 )
		{
			MonsterPoint->AdvancedBehavior = 1;		//���뾯����Χ ���������ж�
		}
		else
		{
			if( (MonsterPoint->TimeRecorderEnd - MonsterPoint->TimeRecorderStart) >= dRandomRange(3,8)*0.3)
			{
			
				if(DistanceOfBirthpoint < Radius)
				{
					//DirectionStatusM = dRandomRange(0,9);	//	�������һ������	
					MonsterPoint->Direction = dRandomRange(0,9);
					MonsterPoint->OutOfRadius = 0;	//������������
				}
				else
				{
					//�߳��������ͷ
					if( MonsterPoint->Direction > 4)
					{
						MonsterPoint->Direction -= 4;
					}
					else MonsterPoint->Direction += 4;	

					MonsterPoint->OutOfRadius = 1;	//������������
				}
				char AnimationString[70];	//���������� = ����ģ������ + ������Ϊ +���﷽��(����)
				strcpy(AnimationString,MonsterString[MonsterPoint->MonsterFlag]);	//���������ִ���������� �Ա����춯������ �����ﲻ����IdName)
				strcat(AnimationString,MonsterBehaviorString[0]);	//0--��·
				strcat(AnimationString,DirectionString[MonsterPoint->Direction - 1]);	//д�뷽��
				dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//���Ŷ���
				MonsterPoint->TimeRecorderStart = SystemTimer;//��ʼ��ʱ
				//if(MonsterPoint->Direction != DirectionStatusM){	//ͨ����¼������Բ�׽����ĸı� ���⶯�����ظ�����
				//	MonsterPoint->Direction = DirectionStatusM;
				//	dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//���Ŷ���
				//	MonsterPoint->TimeRecorderStart = SystemTimer;//��ʼ��ʱ
				//}
				int Operator = 1; //ά������
				switch(MonsterPoint->Direction)
				{
				case 1:	MonsterPoint->VelocityX = 0; MonsterPoint->VelocityY = -4;
					break;
				case 2:	MonsterPoint->VelocityX = 4; MonsterPoint->VelocityY = -4; Operator = 1.414;
					break;
				case 3: MonsterPoint->VelocityX = 4; MonsterPoint->VelocityY = 0;
					break;
				case 4:	MonsterPoint->VelocityX = 4; MonsterPoint->VelocityY = 4; Operator = 1.414;
					break;
				case 5: MonsterPoint->VelocityX = 0; MonsterPoint->VelocityY = 4;
					break;
				case 6: MonsterPoint->VelocityX = -4; MonsterPoint->VelocityY = 4; Operator = 1.414;
					break;
				case 7: MonsterPoint->VelocityX = -4; MonsterPoint->VelocityY = 0;
					break;
				case 8:	MonsterPoint->VelocityX = -4; MonsterPoint->VelocityY = -4; Operator = 1.414;
					break;
				}
				dSetSpriteLinearVelocity(MonsterName,MonsterPoint->VelocityX/Operator,MonsterPoint->VelocityY/Operator);
			}
		}
}
void MonsterBehavior(const char* MonsterName,int MonsterStatus)
{
	pMonster MonsterPoint = NULL;	
	MonsterPoint = SearchMonster(MonsterName);	//�ҵ������й������ڽڵ�

	/*ͨ��λ�ò�ֵΪ���� ����Ϊ����ԭ�� ���γɵ�ƽ��ֱ������ϵ ȷ������ĳ���*/
	float BefollowedX = 0,BefollowedY = 0,FollowerX = 0,FollowerY = 0;
	float DeltaX = 0,DeltaY = 0;
	int DirectionStatusM;//���﷽��״̬
	BefollowedX = dGetSpritePositionX("foxer");		//��������λ����Ϣ
	BefollowedY = dGetSpritePositionY("foxer");
	FollowerX = dGetSpritePositionX(MonsterName);	//����λ����Ϣ
	FollowerY = dGetSpritePositionY(MonsterName);
	DeltaX = BefollowedX - FollowerX;		//���ߵ�λ�ò�ֵ(������Ϊ��������)
	DeltaY = BefollowedY - FollowerY;
	if(DeltaX > 3 && DeltaY > 3)
	{
		DirectionStatusM = 4;
	}
	if(DeltaX > 3 && DeltaY < -3)
	{
		DirectionStatusM = 2;
	}
	if(DeltaX < -3 && DeltaY > 3)
	{
		DirectionStatusM = 6;
	}
	if(DeltaX < -3 && DeltaY < -3)
	{
		DirectionStatusM = 8;
	}
	if(DeltaX < 3 && DeltaX > -3 && DeltaY > 0)
	{
		DirectionStatusM = 5;
	}
	if( DeltaX < 3 && DeltaX > -3 && DeltaY < 0)
	{
		DirectionStatusM = 1;
	}
	if( DeltaY < 3 && DeltaY > -3 && DeltaX > 0)
	{
		DirectionStatusM = 3;
	}
	if( DeltaY < 3 && DeltaY > -3 && DeltaX < 0)
	{
		DirectionStatusM = 7;
	}
	char AnimationString[70];	//���������� = ����ģ������ + ������Ϊ +���﷽��(����)
	strcpy(AnimationString,MonsterString[MonsterPoint->MonsterFlag]);	//���������ִ���������� �Ա����춯������ �����ﲻ����IdName)
	strcat(AnimationString,MonsterBehaviorString[MonsterStatus]);	//��������Ϊд��(���ⲿ��������)
	strcat(AnimationString,DirectionString[DirectionStatusM - 1]);	//д�뷽��
	if(MonsterPoint->Direction != DirectionStatusM){	//ͨ����¼������Բ�׽����ĸı� ���⶯�����ظ�����
		MonsterPoint->Direction = DirectionStatusM;
		dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//���Ŷ���
	}

	//switch(MonsterStatus)
	//{
	//	case 0: {		// ����
	//				//strcat(AnimationString,DirectionString[DirectionStatusM - 1]);//д�뷽��
	//				//dAnimateSpritePlayAnimation(MonsterString[MonsterPoint->MonsterFlag],AnimationString,1);
	//				/*MonsterPoint->DesX = dGetSpritePositionX("foxer");
	//				MonsterPoint->DesY = dGetSpritePositionY("foxer");
	//				dSpriteMoveTo(MonsterName,MonsterPoint->DesX,MonsterPoint->DesY,15,1);*/
	//			}
	//		break;
	//	case 1: {
	//				dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);
	//			}
	//		break;
	//	case 2:	dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);
	//		break;
	//	case 3: DeleteMonster(MonsterName);
	//		break;
	//}	
	/*if(DeltaX == 0 && DeltaY > 0)
	{
		dAnimateSpritePlayAnimation(MonsterName,"bossdefendsouth",1);
	}
	if( DeltaX == 0 && DeltaY < 0)
	{
		dAnimateSpritePlayAnimation(MonsterName,"bossdefendnorth",1);
	}
	if( DeltaY == 0 && DeltaX > 0)
	{
		dAnimateSpritePlayAnimation(MonsterName,"bossdefendeast",1);
	}
	if( DeltaY == 0 && DeltaX < 0)
	{
		dAnimateSpritePlayAnimation(MonsterName,"bossdefendwest",1);
	}*/
}
void UpdateInformation(const char* SkillName,const char* MonsterName)
{
	pSkill SkillPoint = SearchSkill(SkillName);
	pMonster MonsterPoint = SearchMonster(MonsterName);
	if(SkillPoint != NULL && MonsterPoint != NULL)
	{
		if(MonsterPoint->AdvancedBehavior == 1)
		{
			MonsterPoint->AdvancedBehavior = 3;
			MonsterReact(MonsterName , SkillName);	//��������
		}

		if(MonsterPoint->AdvancedBehavior == 0)
		{
			MonsterPoint->AdvancedBehavior = 1;
		}
	
		if(MonsterPoint != NULL && SkillPoint != NULL)
		{
			MonsterPoint->HP -= SkillPoint->DeHP; //��Ѫ

			if(MonsterPoint->HP < 2)
			{
				MonsterPoint->AdvancedBehavior = 2;	//����
			}
		}
		if( MonsterPoint->HP <= 0)
		{
			DeleteMonster(MonsterName);
			if(CheckPoint == 2)
			{
				MonsterKill++;
			}
			//MonsterCounter--;
		}
	}
	if( strcmp( MonsterName , "finalboss") == 0 )
	{
		BossHP -= 5;
		if(BossHP <= 0)
		{
			dSetSpriteCollisionReceive( "finalboss", 0 );
			dSetSpriteVisible("finalboss",0);
			dSetSpriteVisible("crystal",1);
		}
	}
	//MonsterPoint->AdvancedBehavior = 3;
	//MonsterReact(MonsterName , SkillName);	//��������
	//DeleteSkill(SkillName);
}
float GetDistance(const char* ObjectOne,const char*ObjectTwo)
{
	float Distance;
	float ObjectOneX,ObjectOneY,ObjectTwoX,ObjectTwoY;
	float DeltaX,DeltaY;
	ObjectOneX = dGetSpritePositionX(ObjectOne);	
	ObjectOneY = dGetSpritePositionY(ObjectOne);

	ObjectTwoX = dGetSpritePositionX(ObjectTwo);	
	ObjectTwoY = dGetSpritePositionY(ObjectTwo);

	DeltaX= ObjectOneX - ObjectTwoX;
	DeltaY= ObjectOneY - ObjectTwoY;

	Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );

	if(Distance>=0){
		return Distance;
	}
	else return 9999;//����α�����
}
int OccupyBirthPoint()
{
	float DistanceOfOne,DistanceOfTwo,DistanceOfThree,DistanceOfFour;
	DistanceOfOne = GetDistance( Foxer.IdName, "birthpoint1");
	DistanceOfTwo = GetDistance( Foxer.IdName, "birthpoint2");
	DistanceOfThree = GetDistance( Foxer.IdName, "birthpoint3");
	DistanceOfFour = GetDistance( Foxer.IdName, "birthpoint4");
	float Distance = 9999;	char* BirthPoint = NULL;
	int flag = 0;
	if(DistanceOfOne < 4)
	{
		Distance = DistanceOfOne;
		BirthPoint = "birthpoint1";
		flag = 1;
	}
	else if( DistanceOfTwo < 4)
	{
		Distance = DistanceOfTwo;
		BirthPoint = "birthpoint2";
		flag = 2;
	}
	else if( DistanceOfThree < 4)
	{
		Distance = DistanceOfThree;
		BirthPoint = "birthpoint3";
		flag = 3;
	}
	else if( DistanceOfFour < 4)
	{
		Distance = DistanceOfFour;
		BirthPoint = "birthpoint4";
		flag = 4;
	}
	
	if(Distance < 4)
	{
		if(Foxer.IsPlaying == 0)
		{
			Foxer.IsPlaying = 1;	//��ʼ����
			dAnimateSpritePlayAnimation(Foxer.IdName,"foxskillxuli",1);
			Foxer.TimeRecorderStart = SystemTimer;
		}

		Foxer.TimeRecorderEnd = SystemTimer;

		if( (Foxer.TimeRecorderEnd - Foxer.TimeRecorderStart) > 1 && Foxer.IsPlaying == 1 && CheckPointFlag[flag - 1] != 1)	//�ɹ�ռ��
		{
			dPlaySound("paizi.ogg",0,1);
			dAnimateSpritePlayAnimation(BirthPoint,"cure",0);
			Foxer.IsPlaying = 0;	//ֹͣ����
			Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;	//��ն�ʱ��
			CheckPointFlag[flag - 1] = 1;
			Occupied++;
		}
	}
	else	//�������ռ������ ������Ч
	{
		Foxer.IsPlaying = 0;	
		Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;
	}

	
	for(int i = 0; i < 4; i++)
	{
		if(CheckPointFlag[i] == 0)
		{
			return 0;
		}
	}
	return 1;
	//Foxer.TimeRecorderEnd = SystemTimer;
	//if( (Foxer.TimeRecorderEnd - Foxer.TimeRecorderEnd) > 1)	//�ɹ�ռ��
	//{
	//	dAnimateSpritePlayAnimation(BirthPoint,"cure",0);
	//	Foxer.IsPlaying = 0;	//ֹͣ����
	//	Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;	//��ն�ʱ��
	//}
}
void SceneFirst()
{
	dSetTextValue("HPvalue", Foxer.HP );
	dSetTextValue("occupied", Occupied );
	int	isDone = 0;
	isDone = OccupyBirthPoint();	//ռ������ز�����Ƿ�ȫ��ռ��
	if(isDone)	
	{
		dSetSpriteVisible("baiyuan",0);		//boss��ʧ
		dSetSpriteVisible("checkpoint",1);		//checkpoint����
	}
	
	if( GetDistance("foxer","checkpoint") <= 3 && isDone)	//ͨ��
	{
		CheckPoint = 2;		//�ڶ���
		g_iGameState = 1;	//������Ϸ��ʼ��
	}
	else
	{	if(MonsterList != NULL)
		{
			pMonster WorkPoint = MonsterList->Next;		
			while(WorkPoint != NULL)	//������������ ������״̬
			{
				switch(WorkPoint->AdvancedBehavior)
				{
				case 0: MonsterPatrolling(WorkPoint->IdName);	//Ѳ��
					break;
				case 1: MonsterStalk(WorkPoint->IdName);		//����
					break;
				case 2: MonsterFlee(WorkPoint->IdName);			//����
					break;
				case 3: MonsterReact(WorkPoint->IdName,NULL);	//Ӧ����Ӧ
					break;
				}
				WorkPoint = WorkPoint->Next;
			}
		}
	}
}
void SceneSecond()
{
	dSetTextValue("HPvalue", Foxer.HP );
	dSetTextValue("Killvalue", MonsterKill );
	TimeRecorderEnd = SystemTimer;
	float DeltaTime = TimeRecorderEnd - TimeRecorderStart;
	if( DeltaTime/5 > 1)
	{
		for(int i = 0 ; i < 4;i++)
		{
			AddMonster(MonsterList,2);
		}
		DeltaTime /= 8; 
		TimeRecorderStart = TimeRecorderEnd = SystemTimer;
	}
	int	isDone = 0;
	if( MonsterKill >= 20)	//ɱ��������һ��ֵ
	{
		isDone = 1;
	}

	if(isDone)	
	{
		dSetSpriteVisible("checkpoint",1);		//checkpoint����
	}
	
	if( GetDistance("foxer","checkpoint") <= 3 && isDone)	//ͨ��
	{
		CheckPoint = 3;		//������
		g_iGameState = 1;	//������Ϸ��ʼ��
	}
	else
	{	
		pMonster WorkPoint = MonsterList->Next;		
		while(WorkPoint != NULL)	//������������ ������״̬
		{
			switch(WorkPoint->AdvancedBehavior)
			{
			case 0: MonsterPatrolling(WorkPoint->IdName);	//Ѳ��
				break;
			case 1: MonsterStalk(WorkPoint->IdName);		//����
				break;
			case 2: MonsterFlee(WorkPoint->IdName);			//����
				break;
			case 3: MonsterReact(WorkPoint->IdName,NULL);	//Ӧ����Ӧ
				break;
			}
			WorkPoint = WorkPoint->Next;
		}
	}
	
}
void SceneFinal()
{
	dSetTextValue("HPvalue", Foxer.HP );
	dSetTextValue("BossHP",BossHP);
	/*if(BossHP < 1)
	{
		CheckPoint = 4;
	}*/
	if(isPlaying == 0)
	{
		for(int i  = 1 ; i <= 18; i++)	
		{
			char * SkillNameOne = dMakeSpriteName("shanshuo",dRandomRange(1,18));//���������������
			int j = dRandomRange(0,1);
			dSetSpriteVisible(SkillNameOne,j);
		}
		isPlaying = 1;
		TimeRecorderStart = SystemTimer;
	}
	TimeRecorderEnd = SystemTimer;
	if( (TimeRecorderEnd - TimeRecorderStart) > 1 && isPlaying == 1)	//�ɹ�ռ��
	{
		isPlaying = 0;	//ֹͣ����
		TimeRecorderEnd = TimeRecorderStart = 0;	//��ն�ʱ��
	}

	for(int i = 1 ; i < 19; i++)
	{
		char * SkillNameTwo = dMakeSpriteName("shanshuo",i);
		if( dIsSpriteVisible( SkillNameTwo ) == 1 )
		{
			int Distance = GetDistance( "foxer", SkillNameTwo);
		/*	if( Distance < 3)
			{
				Foxer.HP -= 5;
			}*/
			if(Distance < 4)
			{
				if(Foxer.IsPlaying == 0)
				{
					Foxer.IsPlaying = 1;	//��ʼ����
					dAnimateSpritePlayAnimation("guanghuan","onfire2",1);
					Foxer.TimeRecorderStart = SystemTimer;
					Foxer.HP -= 5;
					if(Foxer.HP <= 0)
					{
						CheckPoint = 5;
					}
				}
			}
			Foxer.TimeRecorderEnd = SystemTimer;

			if( (Foxer.TimeRecorderEnd - Foxer.TimeRecorderStart) > 1 && Foxer.IsPlaying == 1)
			{
				dAnimateSpritePlayAnimation("guanghuan","dilaozhen",0);
				Foxer.IsPlaying = 0;	//ֹͣ����
				Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;	//��ն�ʱ��

			}
			//else	//����������� ������Ч
			//{
			//	//dAnimateSpritePlayAnimation("guanghuan","onfire2",1);
			//	Foxer.IsPlaying = 0;	
			//	Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;
			//}
		}
	}
	if( GetDistance("crystal","foxer") < 2)
	{
		CheckPoint = 4;
		dStopSound(MusicID);
	}
}