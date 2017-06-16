#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<signal.h>

#define EMBED_BREAKPOINT  asm volatile ("int3;")

FILE *fp;
unsigned char ram[4096];
unsigned char v[16];
unsigned short i_reg;
unsigned short pc;
unsigned char sp;
unsigned short stack[16];
unsigned char sound;
unsigned char delay;
unsigned short opcode;


void opcode_not_implemented() {
  printf("opcode not implemented: 0x%04X", opcode);
  //EMBED_BREAKPOINT;
  exit(0);
}

/* BEGINNING OF OPCODES */
//00E0 - CLS
//Clear the display.
void cls() {
  //Don't do anything since I don't have a rendering method yet
}

//00EE - RET
//Return from a subroutine.
void ret() {
  sp--;
  pc = stack[sp];
}

//0nnn - SYS addr
//Jump to a machine code routine at nnn.
void sys() {
  //Do nothing
}

//1nnn - JP addr
//Jump to location nnn.
void jp_addr() {
  unsigned short bcd = (opcode & 0x0FFF);
  pc = bcd;
}

//2nnn - CALL addr
//Call subroutine at nnn.
void call() {
  //EMBED_BREAKPOINT;
  unsigned short bcd = (opcode & 0x0FFF);
  sp++;
  pc = bcd;
}

//3xkk - SE Vx, byte
//Skip next instruction if Vx = kk.
void se_vx_byte() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  unsigned char cd = (opcode & 0x00FF);
  if (v[b] == cd) {
    pc += 2;
  }
}

//4xkk - SNE Vx, byte
//Skip next instruction if Vx != kk.
void sne_vx_byte() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  unsigned char cd = opcode & 0x00FF;
  if (v[b] != cd) {
    pc += 2;
  }
}

void se_vx_vy() {
  opcode_not_implemented();
}

//6xkk - LD Vx, byte
//The interpreter puts the value kk into register Vx.
void ld_vx_byte() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  unsigned char cd = opcode & 0x00FF;
  v[b] = cd;
}

//7xkk - ADD Vx, byte
//Set Vx = Vx + kk.
void add_vx_byte() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  unsigned char cd = opcode & 0x00FF;
  v[b] += cd;
}

void ld_vx_vy() {
  opcode_not_implemented();
}

void or_vx_vy() {
  opcode_not_implemented();
}

//8xy2 - AND Vx, Vy
//Set Vx = Vx AND Vy.
void and_vx_vy() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  unsigned char c = (opcode & 0x00F0) >> 4;
  v[b] = v[b] & v[c];
}

void xor_vx_vy() {
  opcode_not_implemented();
}

void add_vx_vy() {
  opcode_not_implemented();
}

void sub_vx_vy() {
  opcode_not_implemented();
}

void shr_vx_vy() {
  opcode_not_implemented();
}

void subn_vx_vy() {
  opcode_not_implemented();
}

void shl_vx_vy() {
  opcode_not_implemented();
}

void sne_vx_vy() {
  opcode_not_implemented();
}

//Annn - LD I, addr
//Set I = nnn.
void ld_i_addr() {
  unsigned short bcd = opcode & 0x0FFF;
  i_reg = bcd;
}

void jp_v0_addr() {
  opcode_not_implemented();
}

void rnd() {
  opcode_not_implemented();
}

void drw() {
  opcode_not_implemented();
}

void skp_vx() {
  opcode_not_implemented();
}

void sknp_vx() {
  opcode_not_implemented();
}

void ld_vx_dt() {
  opcode_not_implemented();
}

void ld_vx_k() {
  opcode_not_implemented();
}

void ld_dt_vx() {
  opcode_not_implemented();
}

void ld_st_vx() {
  opcode_not_implemented();
}

//Fx1E - ADD I, Vx
//Set I = I + Vx.
void add_i_vx() {
  unsigned char b = opcode & 0x0F00 >> 8;
  i_reg += v[b];
}

void ld_f_vx() {
  opcode_not_implemented();
}

void ld_b_vx() {
  opcode_not_implemented();
}

//Fx55 - LD [I], Vx
//Store registers V0 through Vx in memory starting at location I.
void ld_iref_vx() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  for(unsigned char i = 0; i < b + 1; i++) {
    i_reg += i;
    ram[i_reg] = v[i];
  }
  i_reg++;
}

//Fx65 - LD Vx, [I]
//Read registers V0 through Vx from memory starting at location I.
void ld_vx_iref() {
  unsigned char b = (opcode & 0x0F00) >> 8;
  for(unsigned char i = 0; i < b + 1; i++) {
    i_reg += i;
    v[i] = ram[i_reg];
  }
  i_reg++;
}
/* END OF OPCODES */


void next_opcode() {
  opcode = (ram[pc] << 8) | ram[pc + 1];
  printf("current opcode: 0x%04X \n", opcode);
  pc += 2;
  stack[sp] = pc;
       if (opcode == 0x00E0) { cls(); }
  else if (opcode == 0x00EE) { ret(); }
  else if (opcode <  0x1000) { sys(); }
  else if (opcode <  0x2000) { jp_addr(); }
  else if (opcode <  0x3000) { call(); }
  else if (opcode <  0x4000) { se_vx_byte(); }
  else if (opcode <  0x5000) { sne_vx_byte(); }
  else if (opcode <  0x6000) { se_vx_vy(); }
  else if (opcode <  0x7000) { ld_vx_byte(); }
  else if (opcode <  0x8000) { add_vx_byte(); }
  else if (opcode <  0x9000) { 
    unsigned char d = opcode & 0x000F;
         if (d == 0x0000) { ld_vx_vy(); }
    else if (d == 0x0001) { or_vx_vy(); }
    else if (d == 0x0002) { and_vx_vy(); }
    else if (d == 0x0003) { xor_vx_vy(); }
    else if (d == 0x0004) { add_vx_vy(); }
    else if (d == 0x0005) { sub_vx_vy(); }
    else if (d == 0x0006) { shr_vx_vy(); }
    else if (d == 0x0007) { subn_vx_vy(); }
    else if (d == 0x000E) { shl_vx_vy(); }
  }
  else if (opcode <  0xA000) { sne_vx_vy(); }
  else if (opcode <  0xB000) { ld_i_addr(); }
  else if (opcode <  0xC000) { jp_v0_addr(); }
  else if (opcode <  0xD000) { rnd(); }
  else if (opcode <  0xE000) { drw(); }
  else if (opcode <  0xF000) {
    unsigned char cd = opcode & 0x00FF;
         if (cd == 0x009E) { skp_vx(); }
    else if (cd == 0x00A1) { sknp_vx(); }
  }
  else { 
    unsigned char cd = opcode & 0x00FF;
         if (cd == 0x0007) { ld_vx_dt(); }
    else if (cd == 0x000A) { ld_vx_k(); }
    else if (cd == 0x0015) { ld_dt_vx(); }
    else if (cd == 0x0018) { ld_st_vx(); }
    else if (cd == 0x001E) { add_i_vx(); }
    else if (cd == 0x0029) { ld_f_vx(); }
    else if (cd == 0x0033) { ld_b_vx(); }
    else if (cd == 0x0055) { ld_iref_vx(); }
    else if (cd == 0x0065) { ld_vx_iref(); }
  }
}

void init_memory() {
  pc = 0x200;
  sp = 0;
  for(unsigned char i = 0; i < 16; i++) {
    v[i] = 0;
    stack[i] = 0;
  }
}


void file_to_mem() {
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  fread(((char*)&ram) + 0x200, fsize, 1, fp);
}

void start() {
  init_memory();
  file_to_mem();
  while (true) {
    next_opcode();
  }
}

int main(int argc, char **argv)
{
  const char *file;
  if ((file = *++argv) != 0)
  {
    if ((fp = fopen(file, "r")) != 0)
    {
      start(fp);
      fclose(fp);
    }
  }
  else {
    printf("No file passed");
  }
  return(0);
}
