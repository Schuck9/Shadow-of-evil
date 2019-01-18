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
int			g_iGameState		=	1;		// 游戏状态，0 -- 游戏结束等待开始状态；1 -- 按下空格键开始，初始化游戏；2 -- 游戏进行中
//
void		GameInit();
void		GameRun( float fDeltaTime );
void		GameEnd();
float fSpeedNorth = 0,fSpeedSouth = 0,fSpeedWest = 0,fSpeedEast = 0;
int DirectionStatus = 0;	//1--北 2--东北 3--东 4--东南 5--南 6--西南 7--西 8--西北 9--左右矛盾 10--上下矛盾
int SkillDirection = 0;
char DirectionString[][50] = {"north", "eastnorth", "east", "eastsouth", "south", "westsouth", "west", "westnorth"};
char MonsterBehaviorString[][50]={"walking", "attack", "defend","die"};// 0--行走 1--攻击 2--防御 3--死亡
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
int CheckPoint = 0;// 1--第一关 2--第二关 3--第三关
int cnt = 0;
float SystemTimer;//系统时钟
float TimeRecorderStart;	//系统计时器
float TimeRecorderEnd;
int isPlaying;
int BossHP;//boss的血量
int HpArray[4] = {4,4,3,4};
int DeHpArray[4] = {1,1,1,1};
int CheckPointFlag[4] = {0};
int MusicID;
typedef struct Skill{
	char* IdName;//技能的序列名
	float posX;//X位置
	float posY;//Y位置
	float VelocityX;//X速度
	float VelocityY;//Y速度
	int isVisible;// 1--可见 2--不可见
	int DeHP;//可以减少HP的值
	int Direction; //方向
	float LifeTime;//单位秒
	Skill* Next;
}skill,*pSkill;

typedef struct Monster{
	char* IdName;//怪物的序列名
	float HP;//血量
	float InitposX;//X位置
	float InitposY;//Y位置
	char* birthpoint;
	float DesX;//目的地X
	float DesY;//目的地Y
	float VelocityX;//X速度
	float VelocityY;//Y速度
	int isVisible;// 1--可见 2--不可见
	float LifeTime;//单位秒
	int MonsterFlag;// 0--白猿 1--树人 2--老妖 3--魔龙
	int Direction;//怪物的正面方向
	float Radius;//活动范围
	int OutOfRadius; //脱离领域
	float AnimationTime;//怪物动画的播放时间
	float TimeRecorderStart;//怪物动画计时器
	float TimeRecorderEnd;
	float TimeRecorderStart2;//怪物动画计时器
	float TimeRecorderEnd2;
	int IsPlaying;//检测动画是否正在播放
	int IsPlaying2;//检测动画是否正在播放
	int AdvancedBehavior;// 0--巡逻 1--跟踪 2--逃跑 3--应激反应
	Monster* Next;
}monster,*pMonster;

typedef struct Player{
	char* IdName;//怪物的序列名
	float HP;//血量
	float TimeRecorderStart;//人物计时器
	float TimeRecorderEnd;
	int IsPlaying;//检测动画是否正在播放
};

int Situation;	//0--人物技能打到怪物 1--怪物技能打到人物 2--技能打到世界边界 
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
void MonsterBehavior(const char* MonsterName,int MonsterStatus);//MonsterStatus决定怪物的行为
void AllMonsterStalk();//怪物的高级行为：跟踪
void MonsterStalk(const char* MonsterName);
void MonsterPatrolling(const char* MonsterName);//怪物的高级行为：巡逻
void MonsterFlee(const char* MonsterName);//怪物的高级行为：逃跑
void MonsterReact(const char* MonsterName, const char* SkillName);//怪物的高级行为：应激反应
void UpdateInformation(const char* SkillName,const char* MonsterName);
float GetDistance(const char* ObjectOne,const char*ObjectTwo);
int OccupyBirthPoint();
void SceneFirst();
void SceneSecond();
void SceneFinal();
void Check();
//==============================================================================
//
// 大体的程序流程为：GameMainLoop函数为主循环函数，在引擎每帧刷新屏幕图像之后，都会被调用一次。


//==============================================================================
//
// 游戏主循环，此函数将被不停的调用，引擎每刷新一次屏幕，此函数即被调用一次
// 用以处理游戏的开始、进行中、结束等各种状态. 
// 函数参数fDeltaTime : 上次调用本函数到此次调用本函数的时间间隔，单位：秒
void GameMainLoop( float	fDeltaTime )
{
	SystemTimer += fDeltaTime;//计时
	switch( g_iGameState )
	{
		// 初始化游戏，清空上一局相关数据
	case 1:
		{
			GameInit();
			g_iGameState	=	2; // 初始化之后，将游戏状态设置为进行中
		}
		break;

		// 游戏进行中，处理各种游戏逻辑
	case 2:
		{
			// TODO 修改此处游戏循环条件，完成正确游戏逻辑
			
			if( g_iGameState	=	2 )
			{
				GameRun( fDeltaTime );
			}
			else
			{
				// 游戏结束。调用游戏结算函数，并把游戏状态修改为结束状态
				g_iGameState	=	0;
				GameEnd();
			}
		}
		break;

		// 游戏结束/等待按空格键开始
	case 0:
	default:
		break;
	};
}

//==============================================================================
//
// 每局开始前进行初始化，清空上一局相关数据
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
				Foxer.HP = 100;	//	人物血量为10
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
					WorkPoint->AdvancedBehavior = 1; //跟踪
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
// 每局游戏进行中
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
// 本局游戏结束
void GameEnd()
{
	
}
//==========================================================================
//
// 鼠标移动
// 参数 fMouseX, fMouseY：为鼠标当前坐标
void OnMouseMove( const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// 鼠标点击
// 参数 iMouseType：鼠标按键值，见 enum MouseTypes 定义
// 参数 fMouseX, fMouseY：为鼠标当前坐标
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
// 鼠标弹起
// 参数 iMouseType：鼠标按键值，见 enum MouseTypes 定义
// 参数 fMouseX, fMouseY：为鼠标当前坐标
void OnMouseUp( const int iMouseType, const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// 键盘按下
// 参数 iKey：被按下的键，值见 enum KeyCodes 宏定义
// 参数 iAltPress, iShiftPress，iCtrlPress：键盘上的功能键Alt，Ctrl，Shift当前是否也处于按下状态(0未按下，1按下)
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
	
	if( iKey == KEY_UP || iKey == KEY_DOWN || iKey == KEY_RIGHT || iKey == KEY_LEFT ) //只输入W S D A
	{
		//	dAnimateSpritePlayAnimation("birthpoint1","cure",0);
			SpriteMoveKeyDown(iKey);
	}
}
//==========================================================================
//
// 键盘弹起
// 参数 iKey：弹起的键，值见 enum KeyCodes 宏定义
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
// 精灵与精灵碰撞
// 参数 szSrcName：发起碰撞的精灵名字
// 参数 szTarName：被碰撞的精灵名字
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
// 精灵与世界边界碰撞
// 参数 szName：碰撞到边界的精灵名字
// 参数 iColSide：碰撞到的边界 0 左边，1 右边，2 上边，3 下边
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
	float Operator = 1;//速度的维稳算子

	switch(iKey)	//	人物行走逻辑
	{
		case KEY_UP:		
			fSpeedNorth = -15.f;
			SkillDirection = DirectionStatus = 1;//北
			dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
			break;
		case KEY_LEFT:
			fSpeedWest = -15.f;	
			SkillDirection = DirectionStatus = 7;//西
			dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0);
			break;
		case KEY_DOWN:	
			fSpeedSouth = 15.f;
			SkillDirection = DirectionStatus = 5;//南
			dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
			break;
		case KEY_RIGHT:
			fSpeedEast = 15.f;	
			SkillDirection = DirectionStatus = 3;//东
			dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);
			break;
	}			//设置方向速度 播放相应方向动画 同时寄存方向状态
	
	if(fSpeedEast != 0 && fSpeedWest ==0)	
	{
		SkillDirection = DirectionStatus = 3;//东
		dAnimateSpritePlayAnimation("foxer","foxerwalkingeast",0);	
	}
	if(fSpeedEast == 0 && fSpeedWest !=0)
	{
		SkillDirection = DirectionStatus = 7;//西
		dAnimateSpritePlayAnimation("foxer","foxerwalkingwest",0); 
	}
    if(fSpeedNorth == 0 && fSpeedSouth!=0)
	{
		SkillDirection = DirectionStatus = 5;//南
		dAnimateSpritePlayAnimation("foxer","foxerwalkingsouth",0);
	}
	if(fSpeedNorth != 0 && fSpeedSouth ==0)
	{	
		SkillDirection = DirectionStatus = 1;//北
		dAnimateSpritePlayAnimation("foxer","foxerwalkingnorth",0);
	}
	if(fSpeedEast != 0 && fSpeedWest !=0)
	{
		SkillDirection = DirectionStatus = 9;//左右矛盾
		if(iKey == KEY_RIGHT)
		{
			dAnimateSpritePlayAnimation("foxer","foxerstandingwest",0);
		}
		else dAnimateSpritePlayAnimation("foxer","foxerstandingeast",0);	//优先播放后按下的按键效果
	}	
	if(fSpeedNorth != 0 && fSpeedSouth !=0)
	{
		DirectionStatus = 10;//上下矛盾
		if(iKey == KEY_UP)
		{
			dAnimateSpritePlayAnimation("foxer","foxerstandingsouth",0);
		}
		else dAnimateSpritePlayAnimation("foxer","foxerstandingnorth",0);
	}
	if( fSpeedSouth > 0 && fSpeedEast > 0)
	{ 
		SkillDirection = DirectionStatus = 4;//东南
		dAnimateSpritePlayAnimation("foxer","foxerwalkingeastsouth",0) ;
		Operator  = 1.414;//根号二
	}
	if( fSpeedSouth > 0 && fSpeedWest < 0)
	{
		SkillDirection = DirectionStatus = 6;//西南
		dAnimateSpritePlayAnimation("foxer","foxerwalkingwestsouth",0);
		Operator  = 1.414;
	}
	if( fSpeedNorth < 0 && fSpeedWest < 0)
	{
		SkillDirection = DirectionStatus = 8;//西北
		dAnimateSpritePlayAnimation("foxer","foxerwalkingwestnorth",0);
		Operator  = 1.414;
	}
	if( fSpeedNorth < 0 && fSpeedEast > 0)
	{
		SkillDirection = DirectionStatus = 2;//东北
		dAnimateSpritePlayAnimation("foxer","foxerwalkingeastnorth",0);
		Operator  = 1.414;
	}

	dSetSpriteLinearVelocity("foxer", (fSpeedWest + fSpeedEast)/Operator, (fSpeedNorth + fSpeedSouth)/Operator);//设置速度 加法是为了统一代码 化繁为简
}
void SpriteMoveKeyUp(int iKey)
{
	switch(iKey) //按键弹起 速度置零
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
	
	switch(DirectionStatus) //防止人物漂移和行走站立状态改变引起的bug
	{
		case 1:
			if(fSpeedSouth !=0) //在往北走的同时按下往南的按键 而后播放静止动画 之后松开往北键 人物往南走 由于往南的按键早已经按下 程序无法再进入OnKeyDown 导致往南行走动画无法播放 最终人物以之前的静止动画往南移动 形成漂移Bug
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
	dSetSpriteLinearVelocity("foxer", (fSpeedWest + fSpeedEast), (fSpeedNorth+fSpeedSouth));//设置速度
}
void SpriteReleaseSkill(int iKey)
{
	int WhichSkill ;//技能选择判断变量
	switch(iKey) //选择技能
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
	//	// 随机一个位置
	//	iPosX	=	dRandomRange( dGetScreenLeft(), dGetScreenRight() );
	//	iPosY	=	dRandomRange( dGetScreenTop(), dGetScreenBottom() );
	//	dSetSpritePosition( WorkPoint->Name, (float)iPosX, (float)iPosY );

	//	WorkPoint = WorkPoint->Next;
	//	SkillNumber++;
	//}
}
pSkill CreateSkill(int WhichSkill)
{
	int skillNumber = 1;//技能的个数

	pSkill Head = (pSkill)malloc(sizeof(Skill));//分配一个节点的内存
	if (Head == NULL)
	{
		exit(-1);
	}
	pSkill Tail = Head;	//定义链表尾部 开始的时候指向首节点
	Tail->Next = NULL;	//最后节点的指针指向为空

	for (int i = 0; i < skillNumber; i++)
	{
		pSkill New = (pSkill)malloc(sizeof(Skill));//创建新节点
		if (New == NULL)
		{
			exit(-1);
		}
		SetSkillInformation(New,WhichSkill);//输入技能信息

		Tail->Next = New;//将原末节点的尾指针指向新节点 即连接节点
		New->Next = NULL;//尾节点内指针指向空
		Tail = New;//新节点成为尾部 工作指针Tail

	}
	SkillCounter++;
	return Head;//返回链表
}
void AddSkill(pSkill SkillList,int WhichSkill) 
{
	pSkill WorkPoint = SkillList;
	while (WorkPoint->Next != NULL)
	{
		WorkPoint = WorkPoint->Next;
	}	//先将工作指针指向最后一个节点

	pSkill New = (pSkill)malloc( sizeof(Skill) );//创建一个新的技能节点
	SetSkillInformation(New,WhichSkill);//输入技能信息
	New->Next = NULL;
	WorkPoint->Next = New;
	SkillCounter++;
}

void SetSkillInformation(pSkill New,int WhichSkill)
{
	float Operator = 1;		//维稳算子
	SkillSpeedNorth = SkillSpeedSouth = SkillSpeedEast = SkillSpeedWest = 0;//清空上个精灵的记录
	//char Direction[50];		//技能(含方向)的字符串
	//char* SkillTemplate[4] = {"heartenergy","zhuandong","jingu","utimateskill"};
	New->posX =  dGetSpriteLinkPointPosX("foxer", 2 );		//设置技能的初始X坐标
	New->posY =  dGetSpriteLinkPointPosY("foxer", 2 );		//设置技能的初始Y坐标
	New->DeHP = DeHpArray[WhichSkill];
	New->Direction = SkillDirection;	//记录速度的方向
	//New->IdName	=	dMakeSpriteName( SkillTemplate[WhichSkill], SkillCounter );		//赋予技能的IdName
	//dCloneSprite( SkillTemplate[WhichSkill], New->IdName );		//根据选择克隆精灵 产生技能动画
	New->IdName	=	dMakeSpriteName( SkillString[WhichSkill], SkillCounter );		//赋予技能的IdName
	dCloneSprite( SkillString[WhichSkill], New->IdName );		//根据选择克隆精灵 产生技能动画

	dSetSpritePosition( New->IdName, New->posX, New->posY );		//设置精灵的位置
	dSetSpriteWorldLimit(New->IdName, WORLD_LIMIT_NULL, ScreenLeft, ScreenTop, ScreenRight, ScreenBottom);//设置窗口为技能的世界边界
	dSetSpriteCollisionSend(New->IdName,true);

	if((fSpeedEast+fSpeedWest) == 0 &&(fSpeedNorth+fSpeedSouth) == 0)	//设置精灵的速度(矢量)
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
	
	switch(WhichSkill)//修改技能效果
	{
		case 0: 
			char AnimationString[50];	//动画的名字 = 技能名字 + 方向
			strcpy(AnimationString,SkillString[WhichSkill]);	//将技能名字存放在数组中 以便制造动画名字 
			strcat(AnimationString,DirectionString[SkillDirection-1]);	//写入方向
			//Direction = strcat(SkillString[WhichSkill],DirectionString[SkillDirection-1]);//拼接技能名(附带方向)
			dAnimateSpritePlayAnimation(New->IdName,AnimationString,0);//播放相应动画
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
//	float Operator = 1;		//维稳算子
//	SkillSpeedNorth = SkillSpeedSouth = SkillSpeedEast = SkillSpeedWest = 0;//清空上个精灵的记录
//	char* Direction = NULL;		//技能(含方向)的字符串
//	//char* SkillTemplate[4] = {"heartenergy","zhuandong","jingu","utimateskill"};
//	New->posX =  dGetSpriteLinkPointPosX("foxer", 2 );		//设置技能的初始X坐标
//	New->posY =  dGetSpriteLinkPointPosY("foxer", 2 );		//设置技能的初始Y坐标
//	New->DeHP = DeHpArray[WhichSkill];
//	//New->IdName	=	dMakeSpriteName( SkillTemplate[WhichSkill], SkillCounter );		//赋予技能的IdName
//	//dCloneSprite( SkillTemplate[WhichSkill], New->IdName );		//根据选择克隆精灵 产生技能动画
//	New->IdName	=	dMakeSpriteName( SkillString[WhichSkill], SkillCounter );		//赋予技能的IdName
//	dCloneSprite( SkillString[WhichSkill], New->IdName );		//根据选择克隆精灵 产生技能动画
//
//	dSetSpritePosition( New->IdName, New->posX, New->posY );		//设置精灵的位置
//	dSetSpriteWorldLimit(New->IdName, WORLD_LIMIT_NULL, ScreenLeft, ScreenTop, ScreenRight, ScreenBottom);//设置窗口为技能的世界边界
//	dSetSpriteCollisionSend(New->IdName,true);
//
//	if((fSpeedEast+fSpeedWest) == 0 &&(fSpeedNorth+fSpeedSouth) == 0)	//设置精灵的速度(矢量)
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
//	switch(WhichSkill)//修改技能效果
//	{
//		case 0: 
//			Direction = strcat(SkillString[WhichSkill],DirectionString[DirectionStatus-1]);//拼接技能名(附带方向)
//			dAnimateSpritePlayAnimation(New->IdName,Direction,0);//播放相应动画
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
//void DeleteSkill(pSkill ObjectPre ,int flag)// 删除单一精灵:flag == 0  删除所有精灵：flag ==1 
//{
//	SearchSkill();
//
//	pSkill ShipmentPoint = ObjectPre->Next;//目标节点 保留控制手段
//	char* SpriteName = ShipmentPoint->IdName;
//	switch (flag)
//	{
//	 case 0:
//	 {
//		 dDeleteSprite(SpriteName);
//		 ObjectPre->Next= ShipmentPoint->Next;//前驱节点的尾指针指向参考节点后节点
//		 free(ShipmentPoint); //释放参考节点的空间
//		 ShipmentPoint = NULL; //防止产生空指针
//		 if(Counter%2 == 0)
//		 {dSetSpriteLinearVelocity("jijione",20,0);}
//		 else
//		{dSetSpriteLinearVelocity("jijione",-20,0);}
//		 Counter++;
//	 }
//		 break;
//	 case1:
//		 while (ObjectPre != NULL) {
//			 ShipmentPoint = ObjectPre;        //Shipment指向要删除的节点
//			 ObjectPre = ObjectPre->Next;//移动到下一个节点
//			 free(ShipmentPoint);    //删除原节点
//		 }
//		 break;
//	}
//}
void DeleteSkill(const char* SkillName)	
{
	pSkill SkillPoint= SearchSkill(SkillName);	//找到技能的节点 
	pSkill ObjectPre = NULL;	//技能的前驱节点
	pSkill ShipmentPoint = SkillPoint;	//中转节点 保留控制手段
	ObjectPre = SkillList;	//准备遍历链表
	while( ObjectPre->Next != SkillPoint)
	{
		ObjectPre = ObjectPre->Next;
	}
	dDeleteSprite(SkillPoint->IdName);	//地图上删除技能
	ObjectPre->Next= ShipmentPoint->Next;	//前驱节点的尾指针指向参考节点后节点
	free(ShipmentPoint);	//释放参考节点的空间
	ShipmentPoint = NULL;	//防止产生空指针

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
	pMonster MonsterPoint= SearchMonster(MonsterName);	//找到怪物的节点 
	pMonster ObjectPre = NULL;	//怪物的前驱节点
	pMonster ShipmentPoint = MonsterPoint;	//中转节点 保留控制手段
	ObjectPre = MonsterList;	//准备遍历链表
	while( ObjectPre->Next != MonsterPoint)
	{
		ObjectPre = ObjectPre->Next;
	}
	dDeleteSprite(MonsterPoint->IdName);	//地图上删除怪物
	ObjectPre->Next= ShipmentPoint->Next;	//前驱节点的尾指针指向参考节点后节点
	free(ShipmentPoint);	//释放参考节点的空间
	ShipmentPoint = NULL;	//防止产生空指针

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
		return NULL;	//未建立指针列表
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
			SkillSearchPoint = SkillSearchPoint->Next;	//未找到则查找下一个节点
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
			MonsterSearchPoint = MonsterSearchPoint->Next;	//未找到则查找下一个节点
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
//			SkillSearchPoint = SkillSearchPoint->Next;	//未找到则查找下一个节点
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
//			MonsterSearchPoint = MonsterSearchPoint->Next;	//未找到则查找下一个节点
//		}
//	}
//	switch(Situation)
//	{
//		case 0:{	//人物技能打到怪物
//				//
//		   }
//			break;
//		case 1:{	//怪物技能打到人物
//				
//			}
//			break;
//		case 2:{	//技能打到世界边界
//					pSkill ObjectPre = SkillList;
//					while (ObjectPre->Next != SkillSearchPoint)
//					{
//						ObjectPre = ObjectPre->Next;
//					}	//寻找目标节点的前驱节点
//					DeleteSprite(ObjectPre,0);	
//		}
//			break;		
//	}
//				
//}
pMonster CreateMonster(int WhichMonster)
{
	int MonsterNumber = 1;//怪物的个数

	pMonster Head = (pMonster)malloc(sizeof(Monster));//分配一个节点的内存
	if (Head == NULL)
	{
		exit(-1);
	}
	pMonster Tail = Head;	//定义链表尾部 开始的时候指向首节点
	Tail->Next = NULL;	//最后节点的指针指向为空

	for (int i = 0; i < MonsterNumber; i++)
	{
		pMonster New = (pMonster)malloc(sizeof(Monster));//创建新节点
		if (New == NULL)
		{
			exit(-1);
		}
		SetMonsterInformation(New,WhichMonster);//输入怪物信息

		Tail->Next = New;//将原末节点的尾指针指向新节点 即连接节点
		New->Next = NULL;//尾节点内指针指向空
		Tail = New;//新节点成为尾部 工作指针Tail

	}
	MonsterCounter++;
	return Head;//返回链表
}
void AddMonster(pMonster MonsterList,int WhichMonster) 
{
	pMonster WorkPoint = MonsterList;
	while (WorkPoint->Next != NULL)
	{
		WorkPoint = WorkPoint->Next;
	}	//先将工作指针指向最后一个节点

	pMonster New = (pMonster)malloc( sizeof(Monster) );//创建一个新的技能节点
	SetMonsterInformation(New,WhichMonster);//输入技能信息
	New->Next = NULL;
	WorkPoint->Next = New;
	MonsterCounter++;
}
void SetMonsterInformation(pMonster New, int WhichMonster)
{
	//New->InitposX =  dRandomRange(ScreenLeft, ScreenRight);		//设置怪物的初始X坐标
	//New->InitposY =  dRandomRange(ScreenTop, ScreenBottom);		//设置怪物的初始Y坐标
	char* birthpointName = dMakeSpriteName("birthpoint",(int)dRandomRange(1,4));
	New->birthpoint = birthpointName;	//记录出生点的名字
	New->InitposX =  dGetSpritePositionX(birthpointName);		//设置怪物的初始X坐标
	New->InitposY =  dGetSpritePositionY(birthpointName)-3;		//设置怪物的初始Y坐标
	New->IdName	=	dMakeSpriteName( MonsterString[WhichMonster], MonsterCounter );		//赋予怪物的IdName
	New->IsPlaying = 0;	//未在播放不循环动画
	New->MonsterFlag = WhichMonster;	//标记哪个怪物
	New->HP = HpArray[WhichMonster];	//怪物的血量值
	New->Radius = 4;	//怪物的领域半径
	New->OutOfRadius = 0;	//在领域内
	New->AdvancedBehavior = 0;//巡逻
	New->TimeRecorderEnd = 0;//怪物计时器初始化
	New->TimeRecorderStart = 0;
	dCloneSprite( MonsterString[WhichMonster], New->IdName );		//根据选择克隆精灵 产生技能动画
	dSetSpritePosition( New->IdName, New->InitposX, New->InitposY );		//设置精灵的位置
	dSetSpriteWorldLimit(New->IdName, WORLD_LIMIT_NULL, ScreenLeft, ScreenTop, ScreenRight, ScreenBottom);		//设置窗口为技能的世界边界
	dSetSpriteCollisionReceive(New->IdName,true);		//接受碰撞
}
void MonsterDie(const char* MonsterName)
{
	//播放死亡动画
	DeleteMonster(MonsterName);//删除怪物相关信息
}
void Check()
{
	//float posX = 0,posY = 0;
	////int IsPlaying = 0;//不在播放
	//posX = dGetSpritePositionX("foxer");
	//posY = dGetSpritePositionY("foxer");
	//pMonster WorkPoint = MonsterList->Next;

	////if(cnt == 0 )
	////{
	////	MonsterBehavior(WorkPoint->IdName,0);
	////}
	//while(WorkPoint != NULL)
	//{	
	//	float DeltaX = posX - dGetSpritePositionX(WorkPoint->IdName);	//人物与怪物的X方向的差值
	//	float DeltaY = posY - dGetSpritePositionY(WorkPoint->IdName);	//人物与怪物的Y方向的差值
	//	if(	DeltaY < 3 && DeltaY > -3 && DeltaX < 3 && DeltaX > -3 && WorkPoint->IsPlaying == 0 )	//判断怪物是否进入以人物为中心的3*3方形区域 进入则播放动画
	//	{
	//		/* 怪物进入人物的方形区域触发攻击动画 */
	//		dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//怪物速度置零
	//		dAnimateSpritePlayAnimation(WorkPoint->IdName,"foxerattack",1);		//播放攻击动画
	//		TimeRecorderStart = SystemTimer;	//计时开始
	//		WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//保存正在播放动画的时间
	//		//IsPlaying = 1;
	//		WorkPoint->IsPlaying = 1;
	//	}
	//	else if(WorkPoint->IsPlaying == 0)// 不播放攻击动画且不在区域内
	//	{
	//		MonsterBehavior(WorkPoint->IdName,0);		//确认跟踪方向
	//		dSpriteMoveTo(WorkPoint->IdName,posX,posY,12,1);		//跟踪
	//	}
	//	TimeRecorderEnd = SystemTimer;
	//	if( (TimeRecorderEnd-TimeRecorderStart) >= 1.5)		//系统时间的差值大于动画时间则播放完毕
	//	{
	//		//IsPlaying = 0;
	//		WorkPoint->IsPlaying = 0;
	//	}
	//		
	//	//if(DeltaY < 3 && DeltaY > -3 && DeltaX < 3 && DeltaX > -3 && IsPlaying == 0)		//判断怪物是否进入以人物为中心的3*3方形区域
	//	//{
	//	//	TimeRecorderStart = SystemTimer;	//计时开始
	//	//	dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//怪物速度置零
	//	//	dAnimateSpritePlayAnimation(WorkPoint->IdName,"foxerattack",1);		//播放攻击动画
	//	//	IsPlaying = 1;	//正在播放动画的标志
	//	//	WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//保存正在播放动画的时间
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
	if(OccupyBirthPoint() && CheckPoint == 1)	//占领出生地 
	{
		dSetSpriteVisible("baiyuan",0);		//boss消失
		dSetSpriteVisible("checkpoint",1);		//checkpoint出现
	}
	
	if( GetDistance("foxer","checkpoint") <= 3 && isDone == 1)	//通关
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
	pMonster MonsterPoint = SearchMonster(MonsterName);		//怪物节点
	pSkill SkillPoint;

	if(SkillName !=NULL)
	{
		SkillPoint = SearchSkill(SkillName);		//找到技能节点以获取技能的方向
		if( MonsterPoint->IsPlaying2 != 0 )	//后退的时间
		{
			int SkillDiretion = SkillPoint->Direction;		//获取技能方向
			int RetreatDiretion = 0;	//怪物后退方向
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
			MonsterPoint->IsPlaying2 = 1;	//开始被动后撤
			MonsterPoint->TimeRecorderStart2 = SystemTimer;	//开始计时

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
	MonsterPoint = SearchMonster(MonsterName);	//找到链表中怪物所在节点

	/*通过位置差值为变量 怪物为坐标原点 所形成的平面直角坐标系 确定怪物的朝向*/
	float BirthPointX = 0,BirthPointY = 0,FollowerX = 0,FollowerY = 0;
	float DeltaX = 0,DeltaY = 0;
	int DirectionStatusM;//怪物方向状态
	char* BirthPoint;	//出生点的名字
	BirthPoint = MonsterPoint->birthpoint;	//出生点的名字
	BirthPointX = MonsterPoint->InitposX;		//出生点位置信息
	BirthPointY = MonsterPoint->InitposY;	
	FollowerX = dGetSpritePositionX(MonsterName);	//怪物位置信息
	FollowerY = dGetSpritePositionY(MonsterName);
	DeltaX = BirthPointX - FollowerX;		//两者的位置差值(被减数为被跟踪者)
	DeltaY = BirthPointY - FollowerY;
	float Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );
	if( Distance <= 2 )
	{
		MonsterPoint->HP = HpArray[MonsterPoint->MonsterFlag];	//回复满血
		//dAnimateSpritePlayAnimation(BirthPoint,"cure",1);	//播放治愈动画
		MonsterPoint->AdvancedBehavior = 1;	//回复跟踪行为
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
		char AnimationString[50];	//动画的名字 = 怪物模板名字 + 怪物行为 +怪物方向(朝向)
		strcpy(AnimationString,MonsterString[MonsterPoint->MonsterFlag]);	//将怪物名字存放在数组中 以便制造动画名字 （这里不能用IdName)
		strcat(AnimationString,MonsterBehaviorString[0]);	//行走
		strcat(AnimationString,DirectionString[DirectionStatusM - 1]);	//写入方向
		if(MonsterPoint->Direction != DirectionStatusM){	//通过记录方向可以捕捉方向的改变 避免动画的重复播放
			MonsterPoint->Direction = DirectionStatusM;
			dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//播放动画
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
		float DeltaX = posX - dGetSpritePositionX(WorkPoint->IdName);	//人物与怪物的X方向的差值
		float DeltaY = posY - dGetSpritePositionY(WorkPoint->IdName);	//人物与怪物的Y方向的差值
		float Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );//怪物与人的距离
		if(	Distance <= 3 && WorkPoint->IsPlaying == 0 )	//判断怪物是否进入以人物为中心的3*3方形区域 进入则播放动画
		{
			/* 怪物进入人物的方形区域触发攻击动画 */
			dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//怪物速度置零
			char AnimationString[70];	//动画的名字 = 怪物模板名字 + 怪物行为 +怪物方向(朝向)
			strcpy(AnimationString,MonsterString[WorkPoint->MonsterFlag]);	//将怪物名字存放在数组中 以便制造动画名字 （这里不能用IdName)
			strcat(AnimationString,MonsterBehaviorString[1]);	//将怪物行为写入(由外部参数决定)
			strcat(AnimationString,DirectionString[WorkPoint->Direction - 1]);	//写入方向
			dAnimateSpritePlayAnimation(WorkPoint->IdName,AnimationString,1);		//播放攻击动画
			//dAnimateSpritePlayAnimation(WorkPoint->IdName,"foxerattack",1);		//播放攻击动画
			WorkPoint->TimeRecorderStart = SystemTimer;	//计时开始
			WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//保存正在播放动画的时间
			WorkPoint->IsPlaying = 1;
			


		}
		else if(WorkPoint->IsPlaying == 0)// 不播放攻击动画且不在区域内
		{
			MonsterBehavior(WorkPoint->IdName,0);		//确认跟踪方向
			dSpriteMoveTo(WorkPoint->IdName,posX,posY,12,1);		//跟踪
		}
		WorkPoint->TimeRecorderEnd = SystemTimer;
		if( (WorkPoint->TimeRecorderEnd-WorkPoint->TimeRecorderStart) >= 1.5)		//系统时间的差值大于动画时间则播放完毕
		{
			WorkPoint->IsPlaying = 0;
			/*清空怪物计时器的数据*/
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
		float DeltaX = posX - dGetSpritePositionX(WorkPoint->IdName);	//人物与怪物的X方向的差值
		float DeltaY = posY - dGetSpritePositionY(WorkPoint->IdName);	//人物与怪物的Y方向的差值
		float Distance = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );//怪物与人的距离

		//if(	DeltaY < 3 && DeltaY > -3 && DeltaX < 3 && DeltaX > -3 && WorkPoint->IsPlaying == 0 )	//判断怪物是否进入以人物为中心的3*3方形区域 进入则播放动画
		if(	Distance <= 3 && WorkPoint->IsPlaying == 0 )	//判断怪物是否进入以人物为中心的3*3方形区域 进入则播放动画
		{
			/* 怪物进入人物的方形区域触发攻击动画 */
			dSetSpriteLinearVelocity(WorkPoint->IdName,0,0);	//怪物速度置零

			char AnimationString[70];	//动画的名字 = 怪物模板名字 + 怪物行为 +怪物方向(朝向)
			strcpy(AnimationString,MonsterString[WorkPoint->MonsterFlag]);	//将怪物名字存放在数组中 以便制造动画名字 （这里不能用IdName)
			strcat(AnimationString,MonsterBehaviorString[1]);	//将怪物行为写入(由外部参数决定)
			strcat(AnimationString,DirectionString[WorkPoint->Direction - 1]);	//写入方向
			dAnimateSpritePlayAnimation(WorkPoint->IdName,AnimationString,1);		//播放攻击动画
			
			Foxer.HP -= 3;//扣血
			if( Foxer.HP <= 0)
			{
				CheckPoint = 5;
			}
			WorkPoint->TimeRecorderStart = SystemTimer;	//计时开始
			WorkPoint->AnimationTime = dGetAnimateSpriteAnimationTime( WorkPoint->IdName );		//保存正在播放动画的时间
			WorkPoint->IsPlaying = 1;
		}
		else if(WorkPoint->IsPlaying == 0)// 不播放攻击动画且不在区域内
		{
			MonsterBehavior(WorkPoint->IdName,0);		//确认跟踪方向
			if(WorkPoint->AdvancedBehavior == 1)
			{
				dSpriteMoveTo(WorkPoint->IdName,posX,posY,12,1);		//跟踪
			}
		}
		WorkPoint->TimeRecorderEnd = SystemTimer;
		if( (WorkPoint->TimeRecorderEnd-WorkPoint->TimeRecorderStart) >= 1.5)		//系统时间的差值大于动画时间则播放完毕
		{
			WorkPoint->IsPlaying = 0;
			/*清空怪物计时器的数据*/
			WorkPoint->TimeRecorderStart = 0;
			WorkPoint->TimeRecorderEnd = 0;
		}
	
}
void MonsterPatrolling(const char* MonsterName)
{
		pMonster MonsterPoint = SearchMonster(MonsterName);
		MonsterPoint->TimeRecorderEnd = SystemTimer;//计时进行中
		int DirectionStatusM;//怪物方向状态
		float RealPosX = 0,RealPosY = 0;	//怪物即时的位置
		float InitPosX = 0,InitPosY = 0;	//怪物出生的位置
		float PlayerPosX = 0, PlayerPosY = 0;	//玩家的位置
		RealPosX = dGetSpritePositionX(MonsterName);
		RealPosY = dGetSpritePositionY(MonsterName);
		InitPosX = MonsterPoint->InitposX;
		InitPosY = MonsterPoint->InitposY;
		PlayerPosX = dGetSpritePositionX("foxer");
		PlayerPosY = dGetSpritePositionY("foxer");

		float DeltaX = PlayerPosX-RealPosX;	//人物与怪物的X方向的差值
		float DeltaY = PlayerPosY - RealPosY;	//人物与怪物的Y方向的差值
		float DistanceOfPlayer = sqrt( DeltaX*DeltaX + DeltaY*DeltaY );//怪物与人的距离
		float Radius = MonsterPoint->Radius;	//领域半径
		float DistanceOfBirthpoint = 0;	//怪物距离出生点的距离
		DistanceOfBirthpoint = sqrt( (RealPosX - InitPosX)*(RealPosX - InitPosX) + (RealPosY - InitPosY)*(RealPosY - InitPosY) );	//距离计算公式
		
		if( DistanceOfPlayer <= 10 )
		{
			MonsterPoint->AdvancedBehavior = 1;		//进入警觉范围 发动跟踪行动
		}
		else
		{
			if( (MonsterPoint->TimeRecorderEnd - MonsterPoint->TimeRecorderStart) >= dRandomRange(3,8)*0.3)
			{
			
				if(DistanceOfBirthpoint < Radius)
				{
					//DirectionStatusM = dRandomRange(0,9);	//	随机给定一个方向	
					MonsterPoint->Direction = dRandomRange(0,9);
					MonsterPoint->OutOfRadius = 0;	//怪物在领域内
				}
				else
				{
					//走出领域则回头
					if( MonsterPoint->Direction > 4)
					{
						MonsterPoint->Direction -= 4;
					}
					else MonsterPoint->Direction += 4;	

					MonsterPoint->OutOfRadius = 1;	//怪物在领域外
				}
				char AnimationString[70];	//动画的名字 = 怪物模板名字 + 怪物行为 +怪物方向(朝向)
				strcpy(AnimationString,MonsterString[MonsterPoint->MonsterFlag]);	//将怪物名字存放在数组中 以便制造动画名字 （这里不能用IdName)
				strcat(AnimationString,MonsterBehaviorString[0]);	//0--走路
				strcat(AnimationString,DirectionString[MonsterPoint->Direction - 1]);	//写入方向
				dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//播放动画
				MonsterPoint->TimeRecorderStart = SystemTimer;//开始计时
				//if(MonsterPoint->Direction != DirectionStatusM){	//通过记录方向可以捕捉方向的改变 避免动画的重复播放
				//	MonsterPoint->Direction = DirectionStatusM;
				//	dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//播放动画
				//	MonsterPoint->TimeRecorderStart = SystemTimer;//开始计时
				//}
				int Operator = 1; //维稳算子
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
	MonsterPoint = SearchMonster(MonsterName);	//找到链表中怪物所在节点

	/*通过位置差值为变量 怪物为坐标原点 所形成的平面直角坐标系 确定怪物的朝向*/
	float BefollowedX = 0,BefollowedY = 0,FollowerX = 0,FollowerY = 0;
	float DeltaX = 0,DeltaY = 0;
	int DirectionStatusM;//怪物方向状态
	BefollowedX = dGetSpritePositionX("foxer");		//被跟踪者位置信息
	BefollowedY = dGetSpritePositionY("foxer");
	FollowerX = dGetSpritePositionX(MonsterName);	//怪物位置信息
	FollowerY = dGetSpritePositionY(MonsterName);
	DeltaX = BefollowedX - FollowerX;		//两者的位置差值(被减数为被跟踪者)
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
	char AnimationString[70];	//动画的名字 = 怪物模板名字 + 怪物行为 +怪物方向(朝向)
	strcpy(AnimationString,MonsterString[MonsterPoint->MonsterFlag]);	//将怪物名字存放在数组中 以便制造动画名字 （这里不能用IdName)
	strcat(AnimationString,MonsterBehaviorString[MonsterStatus]);	//将怪物行为写入(由外部参数决定)
	strcat(AnimationString,DirectionString[DirectionStatusM - 1]);	//写入方向
	if(MonsterPoint->Direction != DirectionStatusM){	//通过记录方向可以捕捉方向的改变 避免动画的重复播放
		MonsterPoint->Direction = DirectionStatusM;
		dAnimateSpritePlayAnimation(MonsterName,AnimationString,1);//播放动画
	}

	//switch(MonsterStatus)
	//{
	//	case 0: {		// 行走
	//				//strcat(AnimationString,DirectionString[DirectionStatusM - 1]);//写入方向
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
			MonsterReact(MonsterName , SkillName);	//后退两步
		}

		if(MonsterPoint->AdvancedBehavior == 0)
		{
			MonsterPoint->AdvancedBehavior = 1;
		}
	
		if(MonsterPoint != NULL && SkillPoint != NULL)
		{
			MonsterPoint->HP -= SkillPoint->DeHP; //扣血

			if(MonsterPoint->HP < 2)
			{
				MonsterPoint->AdvancedBehavior = 2;	//逃跑
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
	//MonsterReact(MonsterName , SkillName);	//后退两步
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
	else return 9999;//返回伪无穷大
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
			Foxer.IsPlaying = 1;	//开始播放
			dAnimateSpritePlayAnimation(Foxer.IdName,"foxskillxuli",1);
			Foxer.TimeRecorderStart = SystemTimer;
		}

		Foxer.TimeRecorderEnd = SystemTimer;

		if( (Foxer.TimeRecorderEnd - Foxer.TimeRecorderStart) > 1 && Foxer.IsPlaying == 1 && CheckPointFlag[flag - 1] != 1)	//成功占领
		{
			dPlaySound("paizi.ogg",0,1);
			dAnimateSpritePlayAnimation(BirthPoint,"cure",0);
			Foxer.IsPlaying = 0;	//停止播放
			Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;	//清空定时器
			CheckPointFlag[flag - 1] = 1;
			Occupied++;
		}
	}
	else	//距离大于占领区域 读秒无效
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
	//if( (Foxer.TimeRecorderEnd - Foxer.TimeRecorderEnd) > 1)	//成功占领
	//{
	//	dAnimateSpritePlayAnimation(BirthPoint,"cure",0);
	//	Foxer.IsPlaying = 0;	//停止播放
	//	Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;	//清空定时器
	//}
}
void SceneFirst()
{
	dSetTextValue("HPvalue", Foxer.HP );
	dSetTextValue("occupied", Occupied );
	int	isDone = 0;
	isDone = OccupyBirthPoint();	//占领出生地并检查是否全部占领
	if(isDone)	
	{
		dSetSpriteVisible("baiyuan",0);		//boss消失
		dSetSpriteVisible("checkpoint",1);		//checkpoint出现
	}
	
	if( GetDistance("foxer","checkpoint") <= 3 && isDone)	//通关
	{
		CheckPoint = 2;		//第二关
		g_iGameState = 1;	//并让游戏初始化
	}
	else
	{	if(MonsterList != NULL)
		{
			pMonster WorkPoint = MonsterList->Next;		
			while(WorkPoint != NULL)	//遍历怪物链表 检查怪物状态
			{
				switch(WorkPoint->AdvancedBehavior)
				{
				case 0: MonsterPatrolling(WorkPoint->IdName);	//巡逻
					break;
				case 1: MonsterStalk(WorkPoint->IdName);		//跟踪
					break;
				case 2: MonsterFlee(WorkPoint->IdName);			//逃跑
					break;
				case 3: MonsterReact(WorkPoint->IdName,NULL);	//应激反应
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
	if( MonsterKill >= 20)	//杀怪数超过一定值
	{
		isDone = 1;
	}

	if(isDone)	
	{
		dSetSpriteVisible("checkpoint",1);		//checkpoint出现
	}
	
	if( GetDistance("foxer","checkpoint") <= 3 && isDone)	//通关
	{
		CheckPoint = 3;		//第三关
		g_iGameState = 1;	//并让游戏初始化
	}
	else
	{	
		pMonster WorkPoint = MonsterList->Next;		
		while(WorkPoint != NULL)	//遍历怪物链表 检查怪物状态
		{
			switch(WorkPoint->AdvancedBehavior)
			{
			case 0: MonsterPatrolling(WorkPoint->IdName);	//巡逻
				break;
			case 1: MonsterStalk(WorkPoint->IdName);		//跟踪
				break;
			case 2: MonsterFlee(WorkPoint->IdName);			//逃跑
				break;
			case 3: MonsterReact(WorkPoint->IdName,NULL);	//应激反应
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
			char * SkillNameOne = dMakeSpriteName("shanshuo",dRandomRange(1,18));//随机决定技能亮灭
			int j = dRandomRange(0,1);
			dSetSpriteVisible(SkillNameOne,j);
		}
		isPlaying = 1;
		TimeRecorderStart = SystemTimer;
	}
	TimeRecorderEnd = SystemTimer;
	if( (TimeRecorderEnd - TimeRecorderStart) > 1 && isPlaying == 1)	//成功占领
	{
		isPlaying = 0;	//停止播放
		TimeRecorderEnd = TimeRecorderStart = 0;	//清空定时器
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
					Foxer.IsPlaying = 1;	//开始播放
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
				Foxer.IsPlaying = 0;	//停止播放
				Foxer.TimeRecorderEnd = Foxer.TimeRecorderStart = 0;	//清空定时器

			}
			//else	//距离大于区域 读秒无效
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