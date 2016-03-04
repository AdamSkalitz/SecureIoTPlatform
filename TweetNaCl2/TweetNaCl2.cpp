#define FOR(i,n) for (i = 0;i < n;++i)
#include "tweetnacl2.h"
#include "Arduino.h"

typedef unsigned char u8;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef long long i64;
typedef i64 gf[16];

//_0 = zero
//_9 = nine
static const u8 _0[16] = {0};
static const u8 _9[32] = {9};
  
  static const gf
  gf0={0},
  gf1 = {1},
  _121665 = {0xDB41,1},
  D = {0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070, 0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203},
  D2 = {0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0, 0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406},
  X = {0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c, 0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169},
  Y = {0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666},
  I = {0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43, 0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83};

static u32 L32(u32 x,int c) { return (x << c) | (x >> (32 - c)); }

static u32 ld32(const u8 *x)
{
  u32 u = x[3];
  u = (u<<8)|x[2];
  u = (u<<8)|x[1];
  return (u<<8)|x[0];
}

sv st32(u8 *x,u32 u)
{
  int i;
  FOR(i,4) { x[i] = u; u >>= 8; }
}

static int vn(const u8 *x,const u8 *y,int n)
{
  u32 i,d = 0;
  FOR(i,n) d |= x[i]^y[i];
  return (1 & ((d - 1) >> 8)) - 1;
}

int crypto_verify_16(const u8 *x,const u8 *y)
{
  return vn(x,y,16);
}

sv core(u8 *out,const u8 *in,const u8 *k,const u8 *c,int h)
{
  u32 w[16],x[16],y[16],t[4];
  int i,j,m;

  FOR(i,4) {
    x[5*i] = ld32(c+4*i);
    x[1+i] = ld32(k+4*i);
    x[6+i] = ld32(in+4*i);
    x[11+i] = ld32(k+16+4*i);
  }

  FOR(i,16) y[i] = x[i];

  FOR(i,20) {
    FOR(j,4) {
      FOR(m,4) t[m] = x[(5*j+4*m)%16];
      t[1] ^= L32(t[0]+t[3], 7);
      t[2] ^= L32(t[1]+t[0], 9);
      t[3] ^= L32(t[2]+t[1],13);
      t[0] ^= L32(t[3]+t[2],18);
      FOR(m,4) w[4*j+(j+m)%4] = t[m];
    }
    FOR(m,16) x[m] = w[m];
  }

  if (h) {
    FOR(i,16) x[i] += y[i];
    FOR(i,4) {
      x[5*i] -= ld32(c+4*i);
      x[6+i] -= ld32(in+4*i);
    }
    FOR(i,4) {
      st32(out+4*i,x[5*i]);
      st32(out+16+4*i,x[6+i]);
    }
  } else
    FOR(i,16) st32(out + 4 * i,x[i] + y[i]);
}

int crypto_core_salsa20(u8 *out,const u8 *in,const u8 *k,const u8 *c)
{
  core(out,in,k,c,0);
  return 0;
}

int crypto_core_hsalsa20(u8 *out,const u8 *in,const u8 *k,const u8 *c)
{
  core(out,in,k,c,1);
  return 0;
}

//thought: In arduino do you have to include an extra char for the null 
//terminating char
static const u8 sigma[17] = "expand 32-byte k";

int crypto_stream_salsa20_xor(u8 *c,const u8 *m,u64 b,const u8 *n,const u8 *k)
{
  u8 z[16],x[64];
  u32 u,i;
  if (!b) return 0;
  FOR(i,16) z[i] = 0;
  FOR(i,8) z[i] = n[i];
  while (b >= 64) {
    crypto_core_salsa20(x,z,k,sigma);
    FOR(i,64) c[i] = (m?m[i]:0) ^ x[i];
    u = 1;
    for (i = 8;i < 16;++i) {
      u += (u32) z[i];
      z[i] = u;
      u >>= 8;
    }
    b -= 64;
    c += 64;
    if (m) m += 64;
  }
  if (b) {
    crypto_core_salsa20(x,z,k,sigma);
    FOR(i,b) c[i] = (m?m[i]:0) ^ x[i];
  }
  return 0;
}

int crypto_stream_salsa20(u8 *c,u64 d,const u8 *n,const u8 *k)
{
  return crypto_stream_salsa20_xor(c,0,d,n,k);
}

int crypto_stream(u8 *c,u64 d,const u8 *n,const u8 *k)
{
  u8 s[32];
  crypto_core_hsalsa20(s,n,k,sigma);
  return crypto_stream_salsa20(c,d,n+16,s);
}

int crypto_stream_xor(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *k)
{
  u8 s[32];
  crypto_core_hsalsa20(s,n,k,sigma);
  return crypto_stream_salsa20_xor(c,m,d,n+16,s);
}

sv add1305(u32 *h,const u32 *c)
{
  u32 j,u = 0;
  FOR(j,17) {
    u += h[j] + c[j];
    h[j] = u & 255;
    u >>= 8;
  }
}

static const u32 minusp[17] = {
  5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252
} ;

int crypto_onetimeauth(u8 *out,const u8 *m,u64 n,const u8 *k)
{
  u32 s,i,j,u,x[17],r[17],h[17],c[17],g[17];

  FOR(j,17) r[j]=h[j]=0;
  FOR(j,16) r[j]=k[j];
  r[3]&=15;
  r[4]&=252;
  r[7]&=15;
  r[8]&=252;
  r[11]&=15;
  r[12]&=252;
  r[15]&=15;

  while (n > 0) {
    FOR(j,17) c[j] = 0;
    for (j = 0;(j < 16) && (j < n);++j) c[j] = m[j];
    c[j] = 1;
    m += j; n -= j;
    add1305(h,c);
    FOR(i,17) {
      x[i] = 0;
      FOR(j,17) x[i] += h[j] * ((j <= i) ? r[i - j] : 320 * r[i + 17 - j]);
    }
    FOR(i,17) h[i] = x[i];
    u = 0;
    FOR(j,16) {
      u += h[j];
      h[j] = u & 255;
      u >>= 8;
    }
    u += h[16]; h[16] = u & 3;
    u = 5 * (u >> 2);
    FOR(j,16) {
      u += h[j];
      h[j] = u & 255;
      u >>= 8;
    }
    u += h[16]; h[16] = u;
  }

  FOR(j,17) g[j] = h[j];
  add1305(h,minusp);
  s = -(h[16] >> 7);
  FOR(j,17) h[j] ^= s & (g[j] ^ h[j]);

  FOR(j,16) c[j] = k[j + 16];
  c[16] = 0;
  add1305(h,c);
  FOR(j,16) out[j] = h[j];
  return 0;
}

int crypto_onetimeauth_verify(const u8 *h,const u8 *m,u64 n,const u8 *k)
{
  u8 x[16];
  crypto_onetimeauth(x,m,n,k);
  return crypto_verify_16(h,x);
}

int crypto_secretbox(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *k)
{
  int i;
  if (d < 32) return -1;
  crypto_stream_xor(c,m,d,n,k);
  crypto_onetimeauth(c + 16,c + 32,d - 32,c);
  FOR(i,16) c[i] = 0;
  return 0;
}

int crypto_secretbox_open(u8 *m,const u8 *c,u64 d,const u8 *n,const u8 *k)
{
  int i;
  u8 x[32];
  if (d < 32) return -1;
  crypto_stream(x,32,n,k);
  if (crypto_onetimeauth_verify(c + 16,c + 32,d - 32,x) != 0) return -1;
  crypto_stream_xor(m,c,d,n,k);
  FOR(i,32) m[i] = 0;
  return 0;
}

sv car25519(gf o)
{
  int i;
  i64 c;
  FOR(i,16) {
    o[i]+=(1LL<<16);
    c=o[i]>>16;
    o[(i+1)*(i<15)]+=c-1+37*(c-1)*(i==15);
    o[i]-=c<<16;
  }
}

sv sel25519(gf p,gf q,int b)
{
  i64 t,i,c=~(b-1);
  FOR(i,16) {
    t= c&(p[i]^q[i]);
    p[i]^=t;
    q[i]^=t;
  }
}

sv pack25519(u8 *o,const gf n)
{
  int i,j,b;
  gf m,t;
  FOR(i,16) t[i]=n[i];
  car25519(t);
  car25519(t);
  car25519(t);
  FOR(j,2) {
    m[0]=t[0]-0xffed;
    for(i=1;i<15;i++) {
      m[i]=t[i]-0xffff-((m[i-1]>>16)&1);
      m[i-1]&=0xffff;
    }
    m[15]=t[15]-0x7fff-((m[14]>>16)&1);
    b=(m[15]>>16)&1;
    m[15]&=0xffff;
    sel25519(t,m,1-b);
  }
  FOR(i,16) {
    o[2*i]=t[i]&0xff;
    o[2*i+1]=t[i]>>8;
  }
}

sv unpack25519(gf o, const u8 *n)
{
  int i;
  FOR(i,16) o[i]=n[2*i]+((i64)n[2*i+1]<<8);
  o[15]&=0x7fff;
}

sv A(gf o,const gf a,const gf b)
{
  int i;
  FOR(i,16) o[i]=a[i]+b[i];
}

sv Z(gf o,const gf a,const gf b)
{
  int i;
  FOR(i,16) o[i]=a[i]-b[i];
}

sv M(gf o,const gf a,const gf b)
{
  i64 i,j,t[31];
  FOR(i,31) t[i]=0;
  FOR(i,16) FOR(j,16) t[i+j]+=a[i]*b[j];
  FOR(i,15) t[i]+=38*t[i+16];
  FOR(i,16) o[i]=t[i];
  car25519(o);
  car25519(o);
}

sv S(gf o,const gf a)
{
  M(o,a,a);
}

sv inv25519(gf o,const gf i)
{
  gf c;
  int a;
  FOR(a,16) c[a]=i[a];
  for(a=253;a>=0;a--) {
    S(c,c);
    if(a!=2&&a!=4) M(c,c,i);
  }
  FOR(a,16) o[a]=c[a];
}

int crypto_scalarmult(u8 *q,const u8 *n,const u8 *p)
{							//y				x
  u8 z[32];
  i64 x[96],r,i;
  gf a,b,c,d,e,f;
  FOR(i,31) z[i]=n[i];
  z[31]=(n[31]&127)|64;
  z[0]&=248;
  unpack25519(x,p);
  FOR(i,16) {
    b[i]=x[i];
    d[i]=a[i]=c[i]=0;
  }
  a[0]=d[0]=1;
  for(i=254;i>=0;--i) {
    r=(z[i>>3]>>(i&7))&1;
    sel25519(a,b,r);
    sel25519(c,d,r);
    A(e,a,c);
    Z(a,a,c);
    A(c,b,d);
    Z(b,b,d);
    S(d,e);
    S(f,a);
    M(a,c,a);
    M(c,b,e);
    A(e,a,c);
    Z(a,a,c);
    S(b,a);
    Z(c,d,f);
    M(a,c,_121665);
    A(a,a,d);
    M(c,c,a);
    M(a,d,f);
    M(d,b,x);
    S(b,e);
    sel25519(a,b,r);
    sel25519(c,d,r);
  }
  FOR(i,16) {
    x[i+32]=a[i];
    x[i+48]=c[i];
    x[i+64]=b[i];
    x[i+80]=d[i];
  }
  inv25519(x+48,x+48);
  M(x+32,x+32,x+48);
  pack25519(q,x+32);
  //printf("hello");
  return 0;
}

int crypto_box_beforenm(u8 *k,const u8 *y,const u8 *x)
{
  u8 s[32];
  crypto_scalarmult(s,x,y);
  return crypto_core_hsalsa20(k,_0,s,sigma);
  //return 0;
}

int crypto_box_afternm(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *k)
{
  return crypto_secretbox(c,m,d,n,k);
}

int crypto_box_open_afternm(u8 *m,const u8 *c,u64 d,const u8 *n,const u8 *k)
{
  return crypto_secretbox_open(m,c,d,n,k);
}

int TweetNaCl2::crypto_box(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *y,const u8 *x)
{
  u8 k[32];
  //printf("hello");
  crypto_box_beforenm(k,y,x);
  return crypto_box_afternm(c,m,d,n,k);
  //return 0;
}

int TweetNaCl2::crypto_box_open(u8 *m,const u8 *c,u64 d,const u8 *n,const u8 *y,const u8 *x)
{
  u8 k[32];
  crypto_box_beforenm(k,y,x);
  return crypto_box_open_afternm(m,c,d,n,k);
}