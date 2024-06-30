/*  Pawn compiler - Error message system
 *  In fact a very simple system, using only 'panic mode'.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2006
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: sc5.c 3579 2006-06-06 13:35:29Z thiadmer $
 */

#include <assert.h>
#if defined	__WIN32__ || defined _WIN32 || defined __MSDOS__
  #include <io.h>
#endif
#if defined LINUX || defined __GNUC__
  #include <unistd.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>     /* ANSI standardized variable argument list functions */
#include <string.h>
#if defined FORTIFY
  #include <alloc/fortify.h>
#endif
#include "sc.h"

static char *errmsg[] = {
  /*001*/  "ожидаемый токен: \"%s\", но найден \"%s\"\n",
  /*002*/  "за каждым \"case\" может следовать только одно утверждение (или выражение)\n",
  /*003*/  "объявление локальной переменной должно появляться в составном блоке\n",
  /*004*/  "функция \"%s\" не реализована\n",
  /*005*/  "функция не может иметь аргументов\n",
  /*006*/  "должно быть присвоено массиву\n",
  /*007*/  "оператор не может быть переопределен\n",
  /*008*/  "должно быть постоянное выражение; предполагается ноль\n",
  /*009*/  "неверный размер массива (отрицательный, нулевой или вне границ)\n",
  /*010*/  "неверная функция или объявление\n",
  /*011*/  "смещение стека/адрес данных должен быть кратен размеру ячейки\n",
  /*012*/  "неверный вызов функции, не является допустимым адресом\n",
  /*013*/  "нет точки входа (нет публичных функций)\n",
  /*014*/  "неверное утверждение; не в switch\n",
  /*015*/  "случай \"default\" должен быть последним случаем в операторе switch\n",
  /*016*/  "несколько defaults в \"switch\"\n",
  /*017*/  "неопределенный символ \"%s\"\n",
  /*018*/  "данные инициализации превышают объявленный размер\n",
  /*019*/  "не является меткой: \"%s\"\n",
  /*020*/  "неверное имя символа \"%s\"\n",
  /*021*/  "символ уже определен: \"%s\"\n",
  /*022*/  "должно быть lvalue (не константа)\n",
  /*023*/  "присваивание массива должно быть простым присваиванием\n",
  /*024*/  "\"break\" или \"continue\" вне контекста\n",
  /*025*/  "заголовок функции отличается от прототипа\n",
  /*026*/  "нет соответствующего \"#if...\"\n",
  /*027*/  "неверная символьная константа\n",
  /*028*/  "неверный индекс (не массив или слишком много индексов): \"%s\"\n",
  /*029*/  "неверное выражение, предполагается ноль\n",
  /*030*/  "составное утверждение не закрыто в конце файла (начало в строке %d)\n",
  /*031*/  "неизвестная директива\n",
  /*032*/  "индекс массива вне границ (переменная \"%s\")\n",
  /*033*/  "массив должен быть индексирован (переменная \"%s\")\n",
  /*034*/  "аргумент не имеет значения по умолчанию (аргумент %d)\n",
  /*035*/  "несоответствие типов аргументов (аргумент %d)\n",
  /*036*/  "пустое утверждение\n",
  /*037*/  "неверная строка (возможно, не завершенная строка)\n",
  /*038*/  "лишние символы в строке\n",
  /*039*/  "константный символ не имеет размера\n",
  /*040*/  "дублирование метки \"case\" (значение %d)\n",
  /*041*/  "неверное многоточие, размер массива не известен\n",
  /*042*/  "неверное сочетание классификаторов\n",
  /*043*/  "символьная константа превышает диапазон для упакованной строки\n",
  /*044*/  "позиционные параметры должны предшествовать всем именованным параметрам\n",
  /*045*/  "слишком много аргументов функции\n",
  /*046*/  "неизвестный размер массива (переменная \"%s\")\n",
  /*047*/  "размеры массивов не совпадают, или массив назначения слишком мал\n",
  /*048*/  "размерности массива не совпадают\n",
  /*049*/  "неверное продолжение строки\n",
  /*050*/  "неверный диапазон\n",
  /*051*/  "неверный индекс, используйте операторы \"[ ]\" на основных размерностях\n",
  /*052*/  "многомерные массивы должны быть полностью инициализированы\n",
  /*053*/  "превышение максимального количества размерностей\n",
  /*054*/  "несоответствующая закрывающая скобка (\"}\")\n",
  /*055*/  "начало тела функции без заголовка функции\n",
  /*056*/  "массивы, локальные переменные и аргументы функций не могут быть публичными (переменная \"%s\")\n",
  /*057*/  "незавершенное выражение перед компиляторной директивой\n",
  /*058*/  "дублирование аргумента; один и тот же аргумент передается дважды\n",
  /*059*/  "аргумент функции не может иметь значение по умолчанию (переменная \"%s\")\n",
  /*060*/  "несколько директив \"#else\" между \"#if ... #endif\"\n",
  /*061*/  "директива \"#elseif\" следует за директивой \"#else\"\n",
  /*062*/  "количество операндов не соответствует оператору\n",
  /*063*/  "тег результата функции оператора \"%s\" должен быть \"%s\"\n",
  /*064*/  "нельзя изменить предопределенные операторы\n",
  /*065*/  "аргумент функции может иметь только один тег (аргумент %d)\n",
  /*066*/  "аргумент функции не может быть аргументом-ссылкой или массивом (аргумент \"%s\")\n",
  /*067*/  "переменная не может быть одновременно ссылкой и массивом (переменная \"%s\")\n",
  /*068*/  "неверная точность рационального числа в #pragma\n",
  /*069*/  "формат рационального числа уже определен\n",
  /*070*/  "поддержка рациональных чисел не была включена\n",
  /*071*/  "пользовательский оператор должен быть объявлен до использования (функция \"%s\")\n",
  /*072*/  "оператор \"sizeof\" недопустим для символов \"function\"\n",
  /*073*/  "аргумент функции должен быть массивом (аргумент \"%s\")\n",
  /*074*/  "шаблон #define должен начинаться с алфавитного символа\n",
  /*075*/  "входная строка слишком длинная (после подстановок)\n",
  /*076*/  "синтаксическая ошибка в выражении или неверный вызов функции\n",
  /*077*/  "неправильное кодирование UTF-8 или поврежденный файл: %s\n",
  /*078*/  "функция использует и \"return\", и \"return <value>\"\n",
  /*079*/  "несоответствие типов возврата (массив и не массив)\n",
  /*080*/  "неизвестный символ или не константный символ (символ \"%s\")\n",
  /*081*/  "нельзя использовать тег как значение по умолчанию для параметра индексированного массива (символ \"%s\")\n",
  /*082*/  "пользовательские операторы и встроенные функции не могут иметь состояний\n",
  /*083*/  "функция или переменная может принадлежать только одному автомату (символ \"%s\")\n",
  /*084*/  "конфликт состояний: одно из состояний уже назначено другой реализации (символ \"%s\")\n",
  /*085*/  "для символа \"%s\" не определены состояния\n",
  /*086*/  "неизвестный автомат \"%s\"\n",
  /*087*/  "неизвестное состояние \"%s\" для автомата \"%s\"\n",
  /*088*/  "публичные переменные и локальные переменные не могут иметь состояний (символ \"%s\")\n",
  /*089*/  "переменные состояния не могут быть инициализированы (символ \"%s\")\n",
  /*090*/  "публичные функции не могут возвращать массивы (символ \"%s\")\n",
  /*091*/  "неоднозначная константа; требуется переопределение тега (символ \"%s\")\n",
  /*092*/  "функции не могут возвращать массивы неизвестного размера (символ \"%s\")\n",
  /*093*/  "оператор \"__addressof\" недопустим в выражениях препроцессора\n"
};

static char *fatalmsg[] = {
  /*100*/  "не удаётся прочитать файл: \"%s\"\n",
  /*101*/  "не удаётся записать в файл: \"%s\"\n",
  /*102*/  "переполнение таблицы: \"%s\"\n",
  /* таблица может быть: таблицей циклов
   *                     таблицей литералов
   *                     буфером постановки
   *                     таблицей опций (файл ответов)
   *                     таблицей оптимизатора пип-кода
   */
  /*103*/  "недостаточно памяти\n",
  /*104*/  "неверная инструкция ассемблера \"%s\"\n",
  /*105*/  "числовое переполнение, превышение емкости\n",
  /*106*/  "скомпилированный скрипт превышает максимальный размер памяти (%ld байт)\n",
  /*107*/  "слишком много сообщений об ошибках на одной строке\n",
  /*108*/  "файл сопоставления кодовых страниц не найден\n",
  /*109*/  "неверный путь: \"%s\"\n",
  /*110*/  "утверждение не выполнено: %s\n",
  /*111*/  "ошибка пользователя: %s\n"

};

static char *warnmsg[] = {
  /*200*/  "символ \"%s\" обрезан до %d символов\n",
  /*201*/  "повторное определение константы/макроса (символ \"%s\")\n",
  /*202*/  "количество аргументов не соответствует определению\n",
  /*203*/  "символ никогда не используется: \"%s\"\n",
  /*204*/  "символу присвоено значение, которое никогда не используется: \"%s\"\n",
  /*205*/  "избыточный код: постоянное выражение равно нулю\n",
  /*206*/  "избыточный тест: постоянное выражение не равно нулю\n",
  /*207*/  "неизвестная директива #pragma\n",
  /*208*/  "функция с результатом-тегом использована до определения, что требует повторного разбора\n",
  /*209*/  "функция \"%s\" должна возвращать значение\n",
  /*210*/  "возможное использование символа до инициализации: \"%s\"\n",
  /*211*/  "возможное непреднамеренное присваивание\n",
  /*212*/  "возможная непреднамеренная побитовая операция\n",
  /*213*/  "несоответствие тегов: ожидалось %s %s, но найдено %s\n",
  /*214*/  "возможно, предполагался аргумент-массив \"const\": \"%s\"\n",
  /*215*/  "выражение не имеет эффекта\n",
  /*216*/  "вложенный комментарий\n",
  /*217*/  "неточное выравнивание\n",
  /*218*/  "использование устаревших прототипов с необязательными точками с запятой\n",
  /*219*/  "локальная переменная \"%s\" затеняет переменную на предыдущем уровне\n",
  /*220*/  "выражение с переопределением тега должно быть в скобках\n",
  /*221*/  "имя метки \"%s\" затеняет имя тега\n",
  /*222*/  "количество цифр превышает точность рационального числа\n",
  /*223*/  "избыточный \"sizeof\": размер аргумента всегда равен 1 (символ \"%s\")\n",
  /*224*/  "неопределённый размер массива в выражении \"sizeof\" (символ \"%s\")\n",
  /*225*/  "недостижимый код\n",
  /*226*/  "переменная присваивается сама себе (символ \"%s\")\n",
  /*227*/  "больше инициализаторов, чем полей перечисления\n",
  /*228*/  "длина инициализатора превышает размер поля перечисления\n",
  /*229*/  "несоответствие тегов индекса (символ \"%s\"): ожидался тег %s, но найден %s\n",
  /*230*/  "нет реализации для состояния \"%s\" в функции \"%s\", нет запасного варианта\n",
  /*231*/  "указание состояния в предварительном объявлении игнорируется\n",
  /*232*/  "выходной файл записан, но с отключенным компактным кодированием\n",
  /*233*/  "переменная состояния \"%s\" затеняет глобальную переменную\n",
  /*234*/  "функция устарела (символ \"%s\") %s\n",
  /*235*/  "у публичной функции отсутствует предварительное объявление (символ \"%s\")\n",
  /*236*/  "неизвестный параметр в подстановке (неверный шаблон #define)\n",
  /*237*/  "предупреждение пользователя: %s\n",
  /*238*/  "бессмысленное сочетание классификаторов (%s)\n",
  /*239*/  "литеральный массив/строка переданы в не-const параметр\n"

};

static char *noticemsg[] = {
  /*001*/  "; вы имели в виду \"%s\"?\n",
  /*002*/  "; переменная состояния вне области видимости\n",
  /*003*/  "; вы имели в виду использовать оператор \"%s\"?\n"

};

#define NUM_WARNINGS    arraysize(warnmsg)
static struct s_warnstack {
  unsigned char disable[(NUM_WARNINGS + 7) / 8]; /* 8 flags in a char */
  struct s_warnstack *next;
} warnstack;

static int errflag;
static int errstart;    /* line number at which the instruction started */
static int errline;     /* forced line number for the error message */
static int errwarn;

/*  error
 *
 *  Outputs an error message (note: msg is passed optionally).
 *  If an error is found, the variable "errflag" is set and subsequent
 *  errors are ignored until lex() finds a semicolon or a keyword
 *  (lex() resets "errflag" in that case).
 *
 *  Global references: inpfname   (referred to only)
 *                     fline      (referred to only)
 *                     fcurrent   (referred to only)
 *                     errflag    (altered)
 */
SC_FUNC int error(long number,...)
{
  static char *prefix[3]={ "error", "fatal error", "warning" };
  static int lastline,errorcount;
  static short lastfile;
  char *msg,*pre;
  va_list argptr;
  char string[128];
  int notice,start;

  /* split the error field between the real error/warning number and an optional
   * "notice" number
   */
  notice=(unsigned long)number >> (sizeof(long)*4);
  number&=(~(unsigned long)0) >> (sizeof(long)*4);
  assert(number>0 && number<300);

  /* errflag is reset on each semicolon.
   * In a two-pass compiler, an error should not be reported twice. Therefore
   * the error reporting is enabled only in the second pass (and only when
   * actually producing output). Fatal errors may never be ignored.
   */
  if ((errflag || sc_status!=statWRITE) && (number<100 || number>=200))
    return 0;

  /* also check for disabled warnings */
  if (number>=200) {
    int index=(number-200)/8;
    int mask=1 << ((number-200)%8);
    if ((warnstack.disable[index] & mask)!=0)
      return 0;
  } /* if */

  if (number<100) {
    assert(number>0 && number<(1+arraysize(errmsg)));
    msg=errmsg[number-1];
    pre=prefix[0];
    errflag=TRUE;       /* set errflag (skip rest of erroneous expression) */
    errnum++;
  } else if (number<200) {
    assert(number>=100 && number<(100+arraysize(fatalmsg)));
    msg=fatalmsg[number-100];
    pre=prefix[1];
    errnum++;           /* a fatal error also counts as an error */
  } else if (errwarn) {
    assert(number>=200 && number<(200+arraysize(warnmsg)));
    msg=warnmsg[number-200];
    pre=prefix[0];
    errflag=TRUE;
    errnum++;
  } else {
    assert(number>=200 && number<(200+arraysize(warnmsg)));
    msg=warnmsg[number-200];
    pre=prefix[2];
    warnnum++;
  } /* if */

  if (notice!=0) {
    assert(notice>0 && notice<(1+arraysize(noticemsg)) && noticemsg[notice-1][0]!='\0');
    strcpy(string,msg);
    strcpy(&string[strlen(string)-1],noticemsg[notice-1]);
    msg=string;
  } /* if */

  assert(errstart<=fline);
  start=errstart;
  if (errline>0)
    start=errline;
  else
    errline=fline;
  assert(start<=errline);
  va_start(argptr,number);
  if (errfname[0]=='\0') {
    if (pc_error((int)number,msg,inpfname,((start==errline) ? -1 : start),errline,argptr)) {
      if (outf!=NULL) {
        pc_closeasm(outf,TRUE);
        outf=NULL;
      } /* if */
      longjmp(errbuf,3);        /* user abort */
    } /* if */
  } else {
    FILE *fp=fopen(errfname,"a");
    if (fp!=NULL) {
      /* don't use `fatal error 111: user error:` redundant prefix */
      if (number==111 || number==237)
        fprintf(fp,"%s(%d) : ",inpfname,errline);
      if (start>=0 && start!=errline)
        fprintf(fp,"%s(%d -- %d) : %s %03d: ",inpfname,start,errline,pre,(int)number);
      else
        fprintf(fp,"%s(%d) : %s %03d: ",inpfname,errline,pre,(int)number);
      vfprintf(fp,msg,argptr);
      fclose(fp);
    } /* if */
  } /* if */
  va_end(argptr);

  if ((number>=100 && number<200) || errnum>25){
    if (errfname[0]=='\0') {
      va_start(argptr,number);
      pc_error(0,"\nCompilation aborted.\n\n",NULL,0,0,argptr);
      va_end(argptr);
    } /* if */
    if (outf!=NULL) {
      pc_closeasm(outf,TRUE);
      outf=NULL;
    } /* if */
    longjmp(errbuf,2);          /* fatal error, quit */
  } /* if */

  errline=-1;
  /* check whether we are seeing many errors on the same line */
  if ((start<0 && lastline!=fline) || lastline<start || lastline>fline || fcurrent!=lastfile)
    errorcount=0;
  lastline=fline;
  lastfile=fcurrent;
  if (number<200 || errwarn)
    errorcount++;
  if (errorcount>=3)
    error(107);         /* too many error/warning messages on one line */

  return 0;
}

SC_FUNC void errorset(int code,int line)
{
  switch (code) {
  case sRESET:
    errflag=FALSE;      /* start reporting errors */
    break;
  case sFORCESET:
    errflag=TRUE;       /* stop reporting errors */
    break;
  case sEXPRMARK:
    errstart=fline;     /* save start line number */
    break;
  case sEXPRRELEASE:
    errstart=-1;        /* forget start line number */
    errline=-1;
    break;
  case sSETPOS:
    errline=line;
    break;
  } /* switch */
}

/* pc_enablewarning()
 * Enables or disables a warning (errors cannot be disabled).
 * Initially all warnings are enabled. The compiler does this by setting bits
 * for the *disabled* warnings and relying on the array to be zero-initialized.
 *
 * Parameter enable can be:
 *  o  0 for disable
 *  o  1 for enable
 *  o  2 for toggle
 */
int pc_enablewarning(int number,warnmode enable)
{
  int index;
  unsigned char mask;

  if (number<200)
    return FALSE;       /* errors and fatal errors cannot be disabled */
  number-=200;
  if (number>=NUM_WARNINGS)
    return FALSE;

  index=number/8;
  mask=(unsigned char)(1 << (number%8));
  switch (enable) {
  case warnDISABLE:
    warnstack.disable[index] |= mask;
    break;
  case warnENABLE:
    warnstack.disable[index] &= (unsigned char)~mask;
    break;
  case warnTOGGLE:
    warnstack.disable[index] ^= mask;
    break;
  } /* switch */

  return TRUE;
}

/* pc_pushwarnings()
 * Saves currently disabled warnings, used to implement #pragma warning push
 */
void pc_pushwarnings(void)
{
  void *p;
  p=calloc(sizeof(struct s_warnstack),1);
  if (p==NULL)
    error(103); /* insufficient memory */
  memmove(p,&warnstack,sizeof(struct s_warnstack));
  warnstack.next=p;
}

/* pc_popwarnings()
 * This function is the reverse of pc_pushwarnings()
 */
void pc_popwarnings(void)
{
  void *p;
  if (warnstack.next==NULL) {
    error(26,"#pragma warning push");   /* no matching "#pragma warning push" */
    return;                             /* nothing to do */
  } /* if */
  p=warnstack.next;
  memmove(&warnstack,p,sizeof(struct s_warnstack));
  free(p);
}

SC_FUNC void warnstack_init(void)
{
  memset(&warnstack,0,sizeof(warnstack));
}

SC_FUNC void warnstack_cleanup(void)
{
  struct s_warnstack *cur,*next;
  if (warnstack.next!=NULL)
    error(1,"#pragma warning pop","-end of file-");
  for (cur=warnstack.next; cur!=NULL; cur=next) {
    next=cur->next;
    free(cur);
  } /* for */
  warnstack.next=NULL;
}

/* pc_seterrorwarnings()
 * Make warnings errors (or not).
 */
void pc_seterrorwarnings(int enable)
{
  errwarn = enable;
}

int pc_geterrorwarnings(void)
{
  return errwarn;
}

/* Implementation of Levenshtein distance, by Lorenzo Seidenari
 */
static int minimum(int a,int b,int c)
{
  int min=a;
  if(b<min)
    min=b;
  if(c<min)
    min=c;
  return min;
}

static int levenshtein_distance(const char *s,const char*t)
{
  //Step 1
  int k,i,j,cost,*d,distance;
  int n=strlen(s);
  int m=strlen(t);
  assert(n>0 && m>0);
  d=(int*)malloc((sizeof(int))*(m+1)*(n+1));
  m++;
  n++;
  //Step 2
  for (k=0;k<n;k++)
    d[k]=k;
  for (k=0;k<m;k++)
    d[k*n]=k;
  //Step 3 and 4
  for (i=1;i<n;i++) {
    for (j=1;j<m;j++) {
      //Step 5
      cost= (tolower(s[i-1])!=tolower(t[j-1]));
      //Step 6
      d[j*n+i]=minimum(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
    } /* for */
  } /* for */
  distance=d[n*m-1];
  free(d);
  return distance;
}

static int get_max_dist(const char *name)
{
  int max_dist=strlen(name)/2; /* for short names, allow only a single edit */
  if (max_dist>MAX_EDIT_DIST)
    max_dist=MAX_EDIT_DIST;
  return max_dist;
}

static int find_closest_symbol_table(const char *name,const symbol *root,int symboltype,symbol **closest_sym)
{
  int dist,max_dist,closest_dist=INT_MAX;
  char symname[2*sNAMEMAX+16];
  symbol *sym;
  assert(closest_sym!=NULL);
  *closest_sym =NULL;
  assert(name!=NULL);
  max_dist=get_max_dist(name);
  for (sym=root->next; sym!=NULL; sym=sym->next) {
    if (sym->fnumber!=-1 && sym->fnumber!=fcurrent)
      continue;
    if ((sym->usage & uDEFINE)==0 && (sym->ident!=iFUNCTN || (sym->usage & (uNATIVE | uPROTOTYPED))!=uPROTOTYPED))
      continue;
    switch (sym->ident)
    {
    case iLABEL:
      if ((symboltype & esfLABEL)==0)
        continue;
      break;
    case iCONSTEXPR:
      if ((symboltype & esfCONST)==0)
        continue;
      break;
    case iVARIABLE:
    case iREFERENCE:
      if ((symboltype & esfVARIABLE)==0)
        continue;
      break;
    case iARRAY:
    case iREFARRAY:
      if ((symboltype & esfARRAY)==0)
        continue;
      break;
    case iFUNCTN:
    case iREFFUNC:
      if ((symboltype & (((sym->usage & uNATIVE)!=0) ? esfNATIVE : esfFUNCTION))==0)
        continue;
      break;
    default:
      assert(0);
    } /* switch */
    funcdisplayname(symname,sym->name);
    if (symname[0] == '\0')
      continue;
    dist=levenshtein_distance(name,symname);
    if (dist>max_dist || dist>=closest_dist)
      continue;
    *closest_sym=sym;
    closest_dist=dist;
    if (closest_dist<=1)
      break;
  } /* for */
  return closest_dist;
}

static symbol *find_closest_symbol(const char *name,int symboltype)
{
  symbol *symloc,*symglb;
  int distloc,distglb;

  if (sc_status==statFIRST)
    return NULL;
  assert(name!=NULL);
  if (name[0]=='\0')
    return NULL;
  distloc=find_closest_symbol_table(name,&loctab,symboltype,&symloc);
  if (distloc<=1)
    distglb=INT_MAX; /* don't bother searching in the global table */
  else
    distglb=find_closest_symbol_table(name,&glbtab,symboltype,&symglb);
  return (distglb<distloc) ? symglb : symloc;
}

static constvalue *find_closest_automaton(const char *name)
{
  constvalue *ptr=sc_automaton_tab.first;
  constvalue *closest_match=NULL;
  int dist,max_dist,closest_dist=INT_MAX;

  assert(name!=NULL);
  max_dist=get_max_dist(name);
  while (ptr!=NULL) {
    if (ptr->name[0]!='\0') {
      dist=levenshtein_distance(name,ptr->name);
      if (dist<closest_dist && dist<=max_dist) {
        closest_match=ptr;
        closest_dist=dist;
        if (closest_dist<=1)
          break;
      } /* if */
    } /* if */
    ptr=ptr->next;
  } /* while */
  return closest_match;
}

static constvalue *find_closest_state(const char *name,int fsa)
{
  constvalue *ptr=sc_state_tab.first;
  constvalue *closest_match=NULL;
  int dist,max_dist,closest_dist=INT_MAX;

  assert(name!=NULL);
  max_dist=get_max_dist(name);
  while (ptr!=NULL) {
    if (ptr->index==fsa && ptr->name[0]!='\0') {
      dist=levenshtein_distance(name,ptr->name);
      if (dist<closest_dist && dist<=max_dist) {
        closest_match=ptr;
        closest_dist=dist;
        if (closest_dist<=1)
          break;
      } /* if */
    } /* if */
    ptr=ptr->next;
  } /* while */
  return closest_match;
}

static constvalue *find_closest_automaton_for_state(const char *statename,int fsa)
{
  constvalue *ptr=sc_state_tab.first;
  constvalue *closest_match=NULL;
  constvalue *automaton;
  const char *fsaname;
  int dist,max_dist,closest_dist=INT_MAX;

  assert(statename!=NULL);
  max_dist=get_max_dist(statename);
  automaton=automaton_findid(ptr->index);
  assert(automaton!=NULL);
  fsaname=automaton->name;
  while (ptr!=NULL) {
    if (fsa!=ptr->index && ptr->name[0]!='\0' && strcmp(statename,ptr->name)==0) {
      automaton=automaton_findid(ptr->index);
      assert(automaton!=NULL);
      dist=levenshtein_distance(fsaname,automaton->name);
      if (dist<closest_dist && dist<=max_dist) {
        closest_match=automaton;
        closest_dist=dist;
        if (closest_dist<=1)
          break;
      } /* if */
    } /* if */
    ptr=ptr->next;
  } /* while */
  return closest_match;
}

SC_FUNC int error_suggest(int number,const char *name,const char *name2,int type,int subtype)
{
  char string[sNAMEMAX*2+2]; /* for "<automaton>:<state>" */
  const char *closest_name=NULL;
  symbol *closest_sym;

  /* don't bother finding the closest names on errors
   * that aren't going to be shown on the 1'st pass
   */
  if ((errflag || sc_status!=statWRITE) && (number<100 || number>=200))
    return 0;

  if (type==estSYMBOL) {
  find_symbol:
    closest_sym=find_closest_symbol(name,subtype);
    if (closest_sym!=NULL) {
      closest_name=closest_sym->name;
      if ((subtype & esfVARIABLE)!=0 && closest_sym->states!=NULL && strcmp(closest_name,name)==0) {
        assert(number==17); /* undefined symbol */
        error(makelong(number,2),name);
        return 0;
      } /* if */
    } /* if */
  } else if (type==estNONSYMBOL) {
    if (tMIDDLE<subtype && subtype<=tLAST) {
      name=sc_tokens[subtype-tFIRST];
      subtype=esfVARCONST;
      goto find_symbol;
    } /* if */
  } else if (type==estAUTOMATON) {
    constvalue *closest_automaton=find_closest_automaton(name);
    if (closest_automaton!=NULL)
      closest_name=closest_automaton->name;
  } else if (type==estSTATE) {
    constvalue *closest_state=find_closest_state(name,subtype);
    if (closest_state!=NULL) {
      closest_name=closest_state->name;
    } else {
      constvalue *closest_automaton=find_closest_automaton_for_state(name,subtype);
      if (closest_automaton!=NULL) {
        sprintf(string,"%s:%s",closest_automaton->name,name);
        closest_name=string;
      } /* if */
    } /* if */
  } else {
    assert(0);
  } /* if */

  if (closest_name==NULL) {
    error(number,name,name2);
  } else if (name2!=NULL) {
    error(makelong(number,1),name,name2,closest_name);
  } else {
    error(makelong(number,1),name,closest_name);
  } /* if */
  return 0;
}
