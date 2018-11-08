#include <reg52.h>
#include <lcd.h>
#include <ds1302.h>
#include <xpt2046.h>

#define uchar unsigned char
#define uint unsigned int
//�������
sbit k1=P1^4;    //ģʽ(��������)k1
sbit k2=P1^5;    //��
sbit k3=P3^2;    //��

sbit beep=P2^1;
uchar mode=0;
uchar temp;
uchar temph=5;
uchar flag;
//ʱ������
sbit k4=P3^3;
sbit k5=P3^4;
sbit k6=P3^5;
uchar  SetState,SetPlace; 
uchar TIME[]={0, 0, 0x13, 0x01, 0x09, 0x06, 0x18};
//��ʱ����(1����)
void delayms(uchar xms)
{
	uchar i,j;
	for(i=xms;i>0;i--)
		for(j=110;j>0;j--);
}

//�������ģ��
void key_pros()//������������������⣩
{
	if(k1==0)
	{
		delayms(100);
		if(k1==0)
		{
			mode++;
			flag=0;
			if(mode==2)
			{
				mode=0;
				flag=1;
			}
			while(!k1);	
		}
	}
	if(mode==1)//k1����һ��
	{
		if(k2==0)
		{
			delayms(100);
			if(k2==0)
			{
				temph+=1;
			}
			while(!k2);	
		}
		
		if(k3==0)
		{
			delayms(100);
			if(k3==0)
			{
				temph-=1;
				if(temph<=0)
				temph=0;
			}
			while(!k3);
		}
	}
}

void data_pros()	//���ݴ�����	����ֵ0-500����λ��
{
	uchar tempbuf[3],temp_buf[3],i;

	tempbuf[0]=temp/100+0x30;		//��λ
	tempbuf[1]=temp%100/10+0x30;	//ʮλ			
	tempbuf[2]=temp%100%10+0x30;	//��λ
	
	temp_buf[0]=temph/100+0x30;
	temp_buf[1]=temph%100/10+0x30;				
	temp_buf[2]=temph%100%10+0x30;

	for(i=0;i<3;i++)
	{
		LcdWriteCom(0x80+i);
		LcdWriteData(tempbuf[i]);
	}

	for(i=0;i<3;i++)
	{
		LcdWriteCom(0xc0+i);
		LcdWriteData(temp_buf[i]);
	}
}

void sound()
{
	uchar i=100;
	while(i--)
	{
		beep=~beep;
		delayms(10000);
	}
}

void baojinpros()	//��������
{
	if(temp>=temph&&flag==1)
	{
		sound();//��������
	}
	else
	{	
		beep=1;//  ����������
	}
}

//ʱ����ʾģ��

void LcdDisplay()
{
	LcdWriteCom(0x84);
	LcdWriteData('2');
	LcdWriteData('0');
	LcdWriteData(0x30+TIME[6]/16);			//��
	LcdWriteData(0x30+(TIME[6]&0x0f));
	LcdWriteData('-');
	LcdWriteData(0x30+TIME[4]/16);			//��
	LcdWriteData(0x30+(TIME[4]&0x0f));
	LcdWriteData('-');
	LcdWriteData(0x30+TIME[3]/16);			//��
	LcdWriteData(0x30+(TIME[3]&0x0f));
	LcdWriteData('-');
	LcdWriteData(0x30+(TIME[5]&0x07));	//����

	LcdWriteCom(0x88+0X40);
	LcdWriteData(0x30+TIME[2]/16);				//ʱ
	LcdWriteData(0x30+(TIME[2]&0x0f));				 
	LcdWriteData('-');
	LcdWriteData(0x30+TIME[1]/16);				//��
	LcdWriteData(0x30+(TIME[1]&0x0f));	
	LcdWriteData('-');
	LcdWriteData(0x30+TIME[0]/16);				//��
	LcdWriteData(0x30+(TIME[0]&0x0f));	
}

void Int0Configuration()// �����ⲿ�ж�0,�����������
{
	//����INT0
	IT0=1;//�����س�����ʽ���½��أ�
	EX0=1;//��INT0���ж�����
	EA=1;//�����ж�	
}

void Int0() interrupt 0	//�ⲿ�ж�0 �жϺ���,�����������	 
{
	delayms(10);
	if(k4==0)//���Ҫ����k4����ʱ�����ã���ô��setstate��ת��setplace���㣨����ѡ��ı�ʲô��
	{
		delayms(100);
		if(k4==0)
		{
			SetState=~SetState;
			SetPlace=0;
			Ds1302Init();			
		}	
	}
}


void main()
{
	uchar i;
	beep=1;
	Int0Configuration();
	LcdInit();	//lcd��ʼ��
	Ds1302Init(); //ʱ��оƬ��ʼ��
	while(1)
	{		
		if(SetState==0)
		{
			Ds1302ReadTime();
		}
		else
		{
			if(k5==0)		//��ⰴ��K5�Ƿ���
			{
				delayms(100);	//��������
				if(k5==0)
				{
					SetPlace++;
					if(SetPlace>=7)	 //�߸�ʱ��
						SetPlace=0;					
				}

				while((i<50)&&(k5==0))	 //��ⰴ���Ƿ��ɿ�
				{
					delayms(10);
					i++;
				}
				i=0;
			}
			if(k6==0)		//��ⰴ��K6�Ƿ���
			{
				delayms(10);	//��������
				if(k6==0)
				{
					TIME[SetPlace]++;
					if((TIME[SetPlace]&0x0f)>9)					 //����BCD�롣
					{
						TIME[SetPlace]=TIME[SetPlace]+6;
					}
					if((TIME[SetPlace]>=0x60)&&(SetPlace<2))		//����ֻ�ܵ�59
					{
						TIME[SetPlace]=0;
					}
					if((TIME[SetPlace]>=0x24)&&(SetPlace==2))		//Сʱֻ�ܵ�23
					{
						TIME[SetPlace]=0;
					}
					if((TIME[SetPlace]>=0x32)&&(SetPlace==3))		//��ֻ�ܵ�31
					{
						TIME[SetPlace]=0;	
					}
					if((TIME[SetPlace]>=0x13)&&(SetPlace==4))		//��ֻ�ܵ�12
					{
						TIME[SetPlace]=0;
					}	
					if((TIME[SetPlace]>=0x8)&&(SetPlace==5))		//��ֻ�ܵ�7
					{
						TIME[SetPlace]=1;
					}			
				}
				
				while((i<50)&&(k6==0))	 //��ⰴ���Ƿ��ɿ�
				{
					delayms(10);
					i++;
				}
				i=0;
				
			}						
		}
		LcdDisplay();//ʱ����ʾ
		key_pros();//����������
		data_pros();//���ݴ�����
		baojinpros(); //����������������̬��
		temp = Read_AD_Data(0xE4);  //��ȡ���ͨ����ֵ A2A1A0=	���������ֲ� X+ͨ��		
	}
	
}
