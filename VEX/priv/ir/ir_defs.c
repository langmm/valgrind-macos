
/*---------------------------------------------------------------*/
/*---                                                         ---*/
/*--- This file (ir_defs.c) is                                ---*/
/*--- Copyright (c) 2004 OpenWorks LLP.  All rights reserved. ---*/
/*---                                                         ---*/
/*---------------------------------------------------------------*/

#include "libvex_basictypes.h"
#include "libvex_ir.h"
#include "libvex.h"

#include "vex_util.h"

/*---------------------------------------------------------------*/
/*--- Printing the IR                                         ---*/
/*---------------------------------------------------------------*/

void ppIRType ( IRType ty )
{
  switch (ty) {
    case Ity_Bit: vex_printf( "Bit"); break;
    case Ity_I8:  vex_printf( "I8"); break;
    case Ity_I16: vex_printf( "I16"); break;
    case Ity_I32: vex_printf( "I32"); break;
    case Ity_I64: vex_printf( "I64"); break;
    default: vpanic("ppIRType");
  }
}

void ppIRConst ( IRConst* con )
{
  switch (con->tag) {
    case Ico_U8:  vex_printf( "0x%x",   (UInt)(con->Ico.U8)); break;
    case Ico_U16: vex_printf( "0x%x",   (UInt)(con->Ico.U16)); break;
    case Ico_U32: vex_printf( "0x%x",   (UInt)(con->Ico.U32)); break;
    case Ico_U64: vex_printf( "0x%llx", (ULong)(con->Ico.U64)); break;
    default: vpanic("ppIRConst");
  }
}

void ppIRTemp ( IRTemp tmp )
{
   if (tmp == INVALID_IRTEMP)
      vex_printf("INVALID_IRTEMP");
   else
      vex_printf( "t%d", (Int)tmp);
}

void ppIROp ( IROp op )
{
   Char* str; 
   IROp  base;
   switch (op) {
      case Iop_Add8 ... Iop_Add64:
         str = "Add"; base = Iop_Add8; break;
      case Iop_Sub8 ... Iop_Sub64:
         str = "Sub"; base = Iop_Sub8; break;
      case Iop_Mul8 ... Iop_Mul64:
         str = "Mul"; base = Iop_Mul8; break;
      case Iop_Or8 ... Iop_Or64:
         str = "Or"; base = Iop_Or8; break;
      case Iop_And8 ... Iop_And64:
         str = "And"; base = Iop_And8; break;
      case Iop_Xor8 ... Iop_Xor64:
         str = "Xor"; base = Iop_Xor8; break;
      case Iop_Shl8 ... Iop_Shl64:
         str = "Shl"; base = Iop_Shl8; break;
      case Iop_Shr8 ... Iop_Shr64:
         str = "Shr"; base = Iop_Shr8; break;
      case Iop_Sar8 ... Iop_Sar64:
         str = "Sar"; base = Iop_Sar8; break;
      case Iop_CmpEQ8 ... Iop_CmpEQ64:
         str = "CmpEQ"; base = Iop_CmpEQ8; break;
      case Iop_CmpNE8 ... Iop_CmpNE64:
         str = "CmpNE"; base = Iop_CmpNE8; break;
      case Iop_Neg8 ... Iop_Neg64:
         str = "Neg"; base = Iop_Neg8; break;
      case Iop_Not8 ... Iop_Not64:
         str = "Not"; base = Iop_Not8; break;
      /* other cases must explicitly "return;" */
      case Iop_8Uto16:  vex_printf("8Uto16");  return;
      case Iop_8Uto32:  vex_printf("8Uto32");  return;
      case Iop_16Uto32: vex_printf("16Uto32"); return;
      case Iop_8Sto16:  vex_printf("8Sto16");  return;
      case Iop_8Sto32:  vex_printf("8Sto32");  return;
      case Iop_16Sto32: vex_printf("16Sto32"); return;
      case Iop_32to1:   vex_printf("32to1");   return;
      case Iop_1Uto8:   vex_printf("1Uto8");   return;
      default:        vpanic("ppIROp(1)");
   }
  
   switch (op - base) {
      case 0: vex_printf(str); vex_printf("8"); break;
      case 1: vex_printf(str); vex_printf("16"); break;
      case 2: vex_printf(str); vex_printf("32"); break;
      case 3: vex_printf(str); vex_printf("64"); break;
      default: vpanic("ppIROp(2)");
   }
}

void ppIRExpr ( IRExpr* e )
{
  Int i;
  switch (e->tag) {
    case Iex_Get:
      vex_printf( "GET(%d,", e->Iex.Get.offset);
      ppIRType(e->Iex.Get.ty);
      vex_printf(")");
      break;
    case Iex_Tmp:
      ppIRTemp(e->Iex.Tmp.tmp);
      break;
    case Iex_Binop:
      ppIROp(e->Iex.Binop.op);
      vex_printf( "(" );
      ppIRExpr(e->Iex.Binop.arg1);
      vex_printf( "," );
      ppIRExpr(e->Iex.Binop.arg2);
      vex_printf( ")" );
      break;
    case Iex_Unop:
      ppIROp(e->Iex.Unop.op);
      vex_printf( "(" );
      ppIRExpr(e->Iex.Unop.arg);
      vex_printf( ")" );
      break;
    case Iex_LDle:
      vex_printf( "LDle:" );
      ppIRType(e->Iex.LDle.ty);
      vex_printf( "(" );
      ppIRExpr(e->Iex.LDle.addr);
      vex_printf( ")" );
      break;
    case Iex_Const:
      ppIRConst(e->Iex.Const.con);
      break;
    case Iex_CCall:
      vex_printf("%s(", e->Iex.CCall.name);
      for (i = 0; e->Iex.CCall.args[i] != NULL; i++) {
        ppIRExpr(e->Iex.CCall.args[i]);
        if (e->Iex.CCall.args[i+1] != NULL)
          vex_printf(",");
      }
      vex_printf("):");
      ppIRType(e->Iex.CCall.retty);
      break;
    default:
      vpanic("ppIExpr");
  }
}

void ppIRStmt ( IRStmt* s )
{
  switch (s->tag) {
    case Ist_Put:
      if (s->Ist.Put.guard) {
         vex_printf("if (");
         ppIRExpr(s->Ist.Put.guard);
         vex_printf( ") PUT(%d) = ", s->Ist.Put.offset);
         ppIRExpr(s->Ist.Put.expr);
      } else {
         vex_printf( "PUT(%d) = ", s->Ist.Put.offset);
         ppIRExpr(s->Ist.Put.expr);
      }
      break;
    case Ist_Tmp:
      ppIRTemp(s->Ist.Tmp.tmp);
      vex_printf( " = " );
      ppIRExpr(s->Ist.Tmp.expr);
      break;
    case Ist_STle:
      vex_printf( "STle(");
      ppIRExpr(s->Ist.STle.addr);
      vex_printf( ") = ");
      ppIRExpr(s->Ist.STle.data);
      break;
    case Ist_Exit:
      vex_printf( "if (" );
      ppIRExpr(s->Ist.Exit.cond);
      vex_printf( ") goto ");
      ppIRConst(s->Ist.Exit.dst);
      break;
    default: 
      vpanic("ppIRStmt");
  }
}

void ppIRNext ( IRNext* nx )
{
  switch (nx->tag) {
    case Inx_UJump: 
      vex_printf( "UJump ");
      ppIRConst(nx->Inx.UJump.dst);
      break;
    case Inx_CJump01:
      vex_printf( "CJump01 (" );
      ppIRExpr(nx->Inx.CJump01.cond);
      vex_printf( ") ");
      ppIRConst(nx->Inx.CJump01.dst0);
      vex_printf( " ");
      ppIRConst(nx->Inx.CJump01.dst1);
      break;
    case Inx_IJump:
      vex_printf( "IJump ");
      ppIRExpr(nx->Inx.IJump.dst);
      break;
    default: 
      vpanic("ppIRNext");
  }
}

void ppIRTypeEnv ( IRTypeEnv* env ) {
   UInt i;
   for (i = 0; i < env->map_used; i++) {
      if (i % 8 == 0)
         vex_printf( "   ");
      ppIRTemp(env->map[i].name);
      vex_printf( ":");
      ppIRType(env->map[i].type);
      if (i % 8 == 7) 
         vex_printf( "\n"); 
      else 
         vex_printf( "   ");
   }
   if (env->map_used > 0 && env->map_used % 8 != 7) 
      vex_printf( "\n"); 
}


void ppIRBB ( IRBB* bb )
{
   IRStmt* s;
   ppIRTypeEnv(bb->tyenv);
   for (s = bb->stmts; s; s = s->link) {
      vex_printf( "   ");
      ppIRStmt(s);
      vex_printf( "\n");
   }
   vex_printf( "   ");
   ppIRNext(bb->next);
   vex_printf( "\n");
}


/*---------------------------------------------------------------*/
/*--- Constructors                                            ---*/
/*---------------------------------------------------------------*/


/* Constructors -- IRConst */

IRConst* IRConst_U8 ( UChar u8 )
{
   IRConst* c = LibVEX_Alloc(sizeof(IRConst));
   c->tag     = Ico_U8;
   c->Ico.U8  = u8;
   return c;
}
IRConst* IRConst_U16 ( UShort u16 )
{
   IRConst* c = LibVEX_Alloc(sizeof(IRConst));
   c->tag     = Ico_U16;
   c->Ico.U16 = u16;
   return c;
}
IRConst* IRConst_U32 ( UInt u32 )
{
   IRConst* c = LibVEX_Alloc(sizeof(IRConst));
   c->tag     = Ico_U32;
   c->Ico.U32 = u32;
   return c;
}
IRConst* IRConst_U64 ( ULong u64 )
{
   IRConst* c = LibVEX_Alloc(sizeof(IRConst));
   c->tag     = Ico_U64;
   c->Ico.U64 = u64;
   return c;
}


/* Constructors -- IRExpr */

IRExpr* IRExpr_Get ( Int off, IRType ty ) {
   IRExpr* e         = LibVEX_Alloc(sizeof(IRExpr));
   e->tag            = Iex_Get;
   e->Iex.Get.offset = off;
   e->Iex.Get.ty     = ty;
   return e;
}
IRExpr* IRExpr_Tmp ( IRTemp tmp ) {
   IRExpr* e      = LibVEX_Alloc(sizeof(IRExpr));
   e->tag         = Iex_Tmp;
   e->Iex.Tmp.tmp = tmp;
   return e;
}
IRExpr* IRExpr_Binop ( IROp op, IRExpr* arg1, IRExpr* arg2 ) {
   IRExpr* e         = LibVEX_Alloc(sizeof(IRExpr));
   e->tag            = Iex_Binop;
   e->Iex.Binop.op   = op;
   e->Iex.Binop.arg1 = arg1;
   e->Iex.Binop.arg2 = arg2;
   return e;
}
IRExpr* IRExpr_Unop ( IROp op, IRExpr* arg ) {
   IRExpr* e       = LibVEX_Alloc(sizeof(IRExpr));
   e->tag          = Iex_Unop;
   e->Iex.Unop.op  = op;
   e->Iex.Unop.arg = arg;
   return e;
}
IRExpr* IRExpr_LDle  ( IRType ty, IRExpr* addr ) {
   IRExpr* e        = LibVEX_Alloc(sizeof(IRExpr));
   e->tag           = Iex_LDle;
   e->Iex.LDle.ty   = ty;
   e->Iex.LDle.addr = addr;
   return e;
}
IRExpr* IRExpr_Const ( IRConst* con ) {
   IRExpr* e        = LibVEX_Alloc(sizeof(IRExpr));
   e->tag           = Iex_Const;
   e->Iex.Const.con = con;
   return e;
}
IRExpr* IRExpr_CCall ( Char* name, IRType retty, IRExpr** args ) {
   IRExpr* e          = LibVEX_Alloc(sizeof(IRExpr));
   e->tag             = Iex_CCall;
   e->Iex.CCall.name  = name;
   e->Iex.CCall.retty = retty;
   e->Iex.CCall.args  = args;
   return e;
}


/* Constructors -- IRStmt */

IRStmt* IRStmt_Put ( IRExpr* guard, Int off, IRExpr* value ) {
   IRStmt* s         = LibVEX_Alloc(sizeof(IRStmt));
   s->tag            = Ist_Put;
   s->link           = NULL;
   s->Ist.Put.guard  = guard;
   s->Ist.Put.offset = off;
   s->Ist.Put.expr   = value;
   return s;
}
IRStmt* IRStmt_Tmp ( IRTemp tmp, IRExpr* expr ) {
   IRStmt* s       = LibVEX_Alloc(sizeof(IRStmt));
   s->tag          = Ist_Tmp;
   s->link         = NULL;
   s->Ist.Tmp.tmp  = tmp;
   s->Ist.Tmp.expr = expr;
   return s;
}
IRStmt* IRStmt_STle ( IRExpr* addr, IRExpr* value ) {
   IRStmt* s        = LibVEX_Alloc(sizeof(IRStmt));
   s->tag           = Ist_STle;
   s->link          = NULL;
   s->Ist.STle.addr = addr;
   s->Ist.STle.data = value;
   return s;
}
IRStmt* IRStmt_Exit ( IRExpr* cond, IRConst* dst ) {
   IRStmt* s        = LibVEX_Alloc(sizeof(IRStmt));
   s->tag           = Ist_Exit;
   s->link          = NULL;
   s->Ist.Exit.cond = cond;
   s->Ist.Exit.dst  = dst;
   return s;
}

/* Constructors -- IRNext */

IRNext* IRNext_UJump ( IRConst* dst ) {
   IRNext* nx        = LibVEX_Alloc(sizeof(IRNext));
   nx->tag           = Inx_UJump;
   nx->Inx.UJump.dst = dst;
   return nx;
}
IRNext* IRNext_CJump01 ( IRExpr* cond, IRConst* dst0, IRConst* dst1 ) {
   IRNext* nx           = LibVEX_Alloc(sizeof(IRNext));
   nx->tag              = Inx_CJump01;
   nx->Inx.CJump01.cond = cond;
   nx->Inx.CJump01.dst0 = dst0;
   nx->Inx.CJump01.dst1 = dst1;
   return nx;
}
IRNext* IRNext_IJump ( IRExpr* dst ) {
   IRNext* nx        = LibVEX_Alloc(sizeof(IRNext));
   nx->tag           = Inx_IJump;
   nx->Inx.IJump.dst = dst;
   return nx;
}


/* Constructors -- IRBB */

IRBB* mkIRBB ( IRTypeEnv* env, IRStmt* stmts, IRNext* next ) {
   IRBB* bb  = LibVEX_Alloc(sizeof(IRBB));
   bb->tyenv = env;
   bb->stmts = stmts;
   bb->next  = next;
   return bb;
}


/*---------------------------------------------------------------*/
/*--- Helper functions for the IR                             ---*/
/*---------------------------------------------------------------*/

IRTypeEnv* newIRTypeEnv ( void )
{
   IRTypeEnv* env = LibVEX_Alloc(sizeof(IRTypeEnv));
   env->map       = NULL;
   env->map_size  = 0;
   env->map_used  = 0;
   return env;
}


void addToIRTypeEnv ( IRTypeEnv* env, IRTemp tmp, IRType ty )
{
   vassert(env);
   vassert(env->map_used >= 0);
   vassert(env->map_size >= 0);
   vassert(env->map_used <= env->map_size);
   if (env->map_used < env->map_size) {
      env->map[env->map_used].name = tmp;
      env->map[env->map_used].type = ty;
      env->map_used++;
   } else {
      Int i;
      Int new_size = env->map_size==0 ? 8 : 2*env->map_size;
      IRTypeEnvMaplet* new_map 
         = LibVEX_Alloc(new_size * sizeof(IRTypeEnvMaplet));
      for (i = 0; i < env->map_used; i++)
         new_map[i] = env->map[i];
      env->map      = new_map;
      env->map_size = new_size;
      return addToIRTypeEnv(env, tmp, ty);
   }
}


IRType lookupIRTypeEnv ( IRTypeEnv* env, IRTemp tmp )
{
   Int i;
   for (i = 0; i < env->map_used; i++)
      if (env->map[i].name == tmp)
         return env->map[i].type;
   vpanic("lookupIRTypeEnv");
}


IRType typeOfIRExpr ( IRTypeEnv* tyenv, IRExpr* e )
{
   switch (e->tag) {
      case Iex_LDle:
         return e->Iex.LDle.ty;
      case Iex_Get:
         return e->Iex.Get.ty;
      case Iex_Tmp:
         return lookupIRTypeEnv(tyenv, e->Iex.Tmp.tmp);
      case Iex_Const:
         switch (e->Iex.Const.con->tag) {
            case Ico_U8:  return Ity_I8;
            case Ico_U16: return Ity_I16;
            case Ico_U32: return Ity_I32;
            case Ico_U64: return Ity_I64;
            default: vpanic("typeOfIRExpr:Iex_Const");
         }
         break;
      case Iex_Binop:
         switch (e->Iex.Binop.op) {
            case Iop_Add32: case Iop_Sub32: case Iop_Mul32: 
            case Iop_Or32:  case Iop_And32: case Iop_Xor32:
            case Iop_Shl32: case Iop_Shr32: case Iop_Sar32:
               return Ity_I32;
            default: break;
         }
         break;
      default:
         break;
   }
   ppIRExpr(e);
   vpanic("typeOfIRExpr");
}

/*---------------------------------------------------------------*/
/*--- end                                           ir_defs.c ---*/
/*---------------------------------------------------------------*/
