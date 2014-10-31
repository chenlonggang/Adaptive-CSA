#include"GAM.h"
GAM::GAM(i32 *superoffset,InArray *offset,u32 *sequence,
		InArray *samples,u8 * zerostable,i32 n,i32 a,
		i32 b,i32 &index):index(index){
	this->superoffset=superoffset;
	this->offset=offset;
	this->sequence=sequence;
	this->samples=samples;
	this->zerostable=zerostable;
	this->n=n;
	this->a=a;
	this->b=b;
	//this->index=index;
	initTables();
}

GAM::~GAM(){
	if(decodevaluenum)
		delete [] decodevaluenum;
	if(decodebitnum)
		delete [] decodebitnum;
	if(decoderesult)
		delete [] decoderesult;
}

void GAM::encode(i32 x){
	u64 y=x;
	i32 zeronums=blogsize(x)-1;
	index=index+zeronums;
	i32 valuewidth=zeronums+1;
	
	i32 anchor=(index>>5);
	i32 overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
}

i32 GAM::decode(i32 & position,i32 &value){
	i32 a=this->zeroRun(position);
	value=getBits(position,a+1);
	position=position+a+1;
	return 2*a+1;
}

i32 GAM::decodeAcc(i32 position,i32 base,i32 num){
	i32 i=0;
	i32 value=0;
	while(i<num){
		decode(position,value);
		base=(base+value)%n;
		i++;
	}
	return base;
}

i32 GAM::leftBoundary(i32 b,i32 l,i32 r,i32 pl){
	i32 ans=0;
	i32 m=0;
	i32 SL=a;
	i32 L=this->b;
	i32 x=samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	i32 position=superoffset[m/SL]+offset->GetValue(b-1);
	i32 d=0;
	while(m<l){
		decode(position,d);
		x=(x+d)%n;
		m++;
	}

	i32 p=0;
	i32 num=0;
	i32 bits=0;
	i32 v=0;
	bool loop=false;
	while(x<pl && m<r){
		loop=true;
		p=getBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum[p];
			v=this->decoderesult[p];
			v=(v==0?(256):v);
			x=(x+v)%n;
		}
		else{
			m++;
			bits=this->decode(position,d);
			x=x+d;
		}
	}
	if(loop){
		if(num!=0){
			x=(x-v+n)%n;
			position=position-this->decodebitnum[p];
			m=m-num;
		}
		else{
			m=m-1;
			x=(x-d+n)%n;
			position=position-bits;
		}
	}
	while(1){
		if(x>=pl){
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		decode(position,d);
		x=(x+d)%n;
	}
	return ans;
}

i32 GAM::rightBoundary(i32 b,i32 l,i32 r,i32 pr){
	i32 m=0;
	i32 L=this->b;
	i32 SL=this->a;
	i32 x=samples->GetValue(b);
	
	i32 ans=l-1;
	
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	i32 d=0;
	i32 position = superoffset[m/SL]+offset->GetValue(m/L);
	while(m<l){
		decode(position,d);
		x=(x+d)%n;
		m++;
	}

	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	i32 v=0;
	while(x<=pr && m<r){
		loop=true;
		p=this->getBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0){
			m++;
			bits=this->decode(position,d);
			x=x+d;
		}
		else{
			m=m+num;
			position=position+this->decodebitnum[p];
			v=this->decoderesult[p];
			v=(v==0?256:v);
			x=(x+v)%n;
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-v)%n;
			position=position-this->decodebitnum[p];
			m=m-num;
		}
		else{
			m=m-1;
			x=(n+x-d)%n;
			position=position-bits;
		}
	}
	while(1){
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		decode(position,d);
		x=x+d;
	}

	return ans;
}
i32 GAM::zeroRun(i32 & position){
	i32 y=0;
	i32 D=16;
	i32 x=getBits(position,D);
	i32 w=y=zerostable[x];
	while(y==D){
		position=position+D;
		x=getBits(position,D);
		y=zerostable[x];
		w=w+y;
	}
	position=position+y;
	return w;
}

i32 GAM::getBits(i32 position,i32 num){
	u32 anchor=(position>>5);
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	i32 overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
}

i32 GAM::blogsize(i32 x){
	i32 len=0;
	while(x>0){
		x=(x>>1);
		len++;
	}
	return len;
}

void GAM::initTables(){
	u8 * Rn=this->decodevaluenum=new u8[1<<16];
	u8 * Rb=this->decodebitnum=new u8[1<<16];
	u8 * Rx=this->decoderesult=new u8[1<<16];
	
	u32 tablesize=(1<<16);
	u32 B[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};
	u32 *temp=this->sequence;
	this->sequence=B;
	i32 b=0;
	i32 num=0;
	i32 x=0;
	i32 d=0;
	i32 preb=0;
	for(u32 i=0;i<tablesize;i++){
		B[0]=(i<<16);
		b=num=x=d=0;
		while(1){
			this->decode(b,d);
			if(b>16)
				break;
			x=x+d;
			num++;
			preb=b;
		}
		Rb[i]=preb;
		Rn[i]=num;
		Rx[i]=x;
//		if(x>255)
//			cout<<"gam x,num,b "<<x<<" "<<num<<" "<<preb<<endl;
	}
	this->sequence=temp;
}

