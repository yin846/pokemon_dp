%{
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     tlcf.l

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: tlcf.l,v $
  Revision 1.20  2006/03/30 23:59:45  yasu
  Change of a writing fiscal year

  Revision 1.19  2006/03/30 03:17:46  yasu
  The measure against a bug sake NUMBER_OVERLAYS will also be accepted.

  Revision 1.18  2006/03/29 13:13:22  yasu
  Support of IF-ELSE-ENDIF

  Revision 1.17  2005/09/01 04:30:52  yasu
  Support for Overlay Group

  Revision 1.16  2005/08/26 11:23:11  yasu
  overlay support for ITCM/DTCM

  Revision 1.15  2005/06/22 00:50:06  yasu
  Updated copyright year

  Revision 1.14  2005/06/20 12:21:48  yasu
  Changed Surffix to Suffix

  Revision 1.13  2004/07/10 04:06:17  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.12  2004/07/02 07:34:53  yasu
  Support OBJECT( )

  Revision 1.11  2004/06/24 07:18:54  yasu
  Support keyword "Autoload"

  Revision 1.10  2004/06/14 11:28:45  yasu
  support section filter "FOREACH.STATIC.OBJECTS=.sectionName"

  Revision 1.9  2004/03/26 05:07:11  yasu
  support variables like as -DNAME=VALUE

  Revision 1.8  2004/02/13 07:13:03  yasu
  support SDK_IRQ_STACKSIZE

  Revision 1.7  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.6  2004/01/15 13:21:03  yasu
  one line <FOREACH.*> command enables

  Revision 1.5  2004/01/15 10:47:56  yasu
  Implementation of a static StackSize

  Revision 1.4  2004/01/14 12:58:47  yasu
  Handle '\r' as newline

  Revision 1.3  2004/01/14 12:38:08  yasu
  Change OverlayName->OverlayDefs

  Revision 1.2  2004/01/07 13:10:17  yasu
  fix all warning at compile -Wall

  Revision 1.1  2004/01/05 02:32:59  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<string.h>
#include	"makelcf.h"
#include	"tlcf.tab.h"

#define	RETURN(x) { debug_printf( "%s [%s]\n", #x, tlcf_yytext ); return(x); }

BOOL  GetLoopStackLevel( void );

void  tlcf_yyerror( const char* str )
{
    if ( tlcf_yytext[0] >= ' ' )
    {
        fprintf( stderr, "makelcf: line %d: %s \'%s\'\n", tlcf_yylineno, str, tlcf_yytext );
    }
    else
    {
        fprintf( stderr, "makelcf: line %d: %s\n", tlcf_yylineno, str );
    }
}

static char* getSection( const char* default_section )
{
    char*  head = NULL;
    char*  p;
    
    for ( p = tlcf_yytext; *p; p ++ )
    {
	switch ( *p )
	{
	case '=':
	case ':':
	    if ( !head )
	    {
		head = p;
	    }
	    break;
	    
	case '>':
	    if ( head )
	    {
	        *p = '\0';
		return strdup( head );
	    }
	    break;

	default:
	    break;
	}
    }
    return default_section ? strdup( default_section ) : NULL;
}

%}
st			[ \t]*
ed			[ \t]*\r?\n
sec			(=[.a-zA-Z]*)?
mod			(:[htre])?
flt			(:[f])?
identifier		[A-Za-z_][A-Za-z_0-9]*
equation		(==|!=|\.EQ\.|\.NE\.|\.GT\.|\.GE\.|\.LT\.|\.LE\.)
value			[^>\n]*

STATIC_OBJ		\<STATIC\.OBJECT{sec}{mod}\>
F_S_OBJS		\<FOREACH\.STATIC\.OBJECTS{sec}{flt}\>
E_S_OBJS		\<END\.STATIC\.OBJECTS{sec}{flt}\>
FOR_STATIC_OBJS_1	^{st}{F_S_OBJS}{ed}
FOR_STATIC_OBJS_2	{F_S_OBJS}
END_STATIC_OBJS_1	^{st}{E_S_OBJS}{ed}
END_STATIC_OBJS_2	{E_S_OBJS}

STATIC_LIB		\<STATIC\.LIBRARY{sec}{mod}\>
F_S_LIBS		\<FOREACH\.STATIC\.LIBRARIES{sec}{flt}\>
E_S_LIBS		\<END\.STATIC\.LIBRARIES{sec}{flt}\>
FOR_STATIC_LIBS_1	^{st}{F_S_LIBS}{ed}
FOR_STATIC_LIBS_2	{F_S_LIBS}
END_STATIC_LIBS_1	^{st}{E_S_LIBS}{ed}
END_STATIC_LIBS_2	{E_S_LIBS}

F_ATLS			\<FOREACH\.AUTOLOADS{sec}{flt}\>
E_ATLS			\<END\.AUTOLOADS{sec}{flt}\>
FOR_AUTOLOADS_1		^{st}{F_ATLS}{ed}
FOR_AUTOLOADS_2		{F_ATLS}
END_AUTOLOADS_1		^{st}{E_ATLS}{ed}
END_AUTOLOADS_2		{E_ATLS}

AUTOLOAD_OBJ		\<AUTOLOAD\.OBJECT{sec}{mod}\>
F_A_OBJS		\<FOREACH\.AUTOLOAD\.OBJECTS{sec}{flt}\>
E_A_OBJS		\<END\.AUTOLOAD\.OBJECTS{sec}{flt}\>
FOR_AUTOLOAD_OBJS_1	^{st}{F_A_OBJS}{ed}
FOR_AUTOLOAD_OBJS_2	{F_A_OBJS}
END_AUTOLOAD_OBJS_1	^{st}{E_A_OBJS}{ed}
END_AUTOLOAD_OBJS_2	{E_A_OBJS}

AUTOLOAD_LIB		\<AUTOLOAD\.LIBRARY{sec}{mod}\>
F_A_LIBS		\<FOREACH\.AUTOLOAD\.LIBRARIES{sec}{flt}\>
E_A_LIBS		\<END\.AUTOLOAD\.LIBRARIES{sec}{flt}\>
FOR_AUTOLOAD_LIBS_1	^{st}{F_A_LIBS}{ed}
FOR_AUTOLOAD_LIBS_2	{F_A_LIBS}
END_AUTOLOAD_LIBS_1	^{st}{E_A_LIBS}{ed}
END_AUTOLOAD_LIBS_2	{E_A_LIBS}

F_OVLS			\<FOREACH\.OVERLAYS{sec}{flt}\>
E_OVLS			\<END\.OVERLAYS{sec}{flt}\>
FOR_OVERLAYS_1		^{st}{F_OVLS}{ed}
FOR_OVERLAYS_2		{F_OVLS}
END_OVERLAYS_1		^{st}{E_OVLS}{ed}
END_OVERLAYS_2		{E_OVLS}

OVERLAY_OBJ		\<OVERLAY\.OBJECT{sec}{mod}\>
F_O_OBJS		\<FOREACH\.OVERLAY\.OBJECTS{sec}{flt}\>
E_O_OBJS		\<END\.OVERLAY\.OBJECTS{sec}{flt}\>
FOR_OVERLAY_OBJS_1	^{st}{F_O_OBJS}{ed}
FOR_OVERLAY_OBJS_2	{F_O_OBJS}
END_OVERLAY_OBJS_1	^{st}{E_O_OBJS}{ed}
END_OVERLAY_OBJS_2	{E_O_OBJS}

OVERLAY_LIB		\<OVERLAY\.LIBRARY{sec}{mod}\>
F_O_LIBS		\<FOREACH\.OVERLAY\.LIBRARIES{sec}{flt}\>
E_O_LIBS		\<END\.OVERLAY\.LIBRARIES{sec}{flt}\>
FOR_OVERLAY_LIBS_1	^{st}{F_O_LIBS}{ed}
FOR_OVERLAY_LIBS_2	{F_O_LIBS}
END_OVERLAY_LIBS_1	^{st}{E_O_LIBS}{ed}
END_OVERLAY_LIBS_2	{E_O_LIBS}

%x IFCOND1
%x IFCOND2
%x IFCOND3
%option pointer
%option yylineno
%option noyywrap
%%

 //===========================================================================
 //    LCF TOKENS
 //===========================================================================

\<IF\.				{ BEGIN IFCOND1; RETURN(tIF_OPEN);  }
<IFCOND1>{identifier}		{ BEGIN IFCOND2; tlcf_yylval.string = strdup(tlcf_yytext); RETURN(tIF_ID);    }
<IFCOND2>{equation}		{ BEGIN IFCOND3; tlcf_yylval.string = strdup(tlcf_yytext); RETURN(tIF_COMP);  }
<IFCOND3>{value}		{                tlcf_yylval.string = strdup(tlcf_yytext); RETURN(tIF_VALUE); }
<IFCOND3>\>			{ BEGIN INITIAL; RETURN(tIF_CLOSE); }
\<ELSE\>			{ RETURN(tIF_ELSE);  }
\<ENDIF\>			{ RETURN(tIF_ENDIF); }

\<STATIC\.NAME\>		{ RETURN(tSTATIC_NAME);            }
\<STATIC\.ADDRESS\>		{ RETURN(tSTATIC_ADDRESS);         }
\<STATIC\.STACKSIZE\>		{ RETURN(tSTATIC_STACKSIZE);       }
\<STATIC\.IRQSTACKSIZE\>	{ RETURN(tSTATIC_IRQSTACKSIZE);    }

{STATIC_OBJ}			{ tlcf_yylval.string = getSection(NULL); RETURN(tSTATIC_OBJECT);          }
{FOR_STATIC_OBJS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_STATIC_OBJECTS); }
{FOR_STATIC_OBJS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_STATIC_OBJECTS); }
{END_STATIC_OBJS_1}		{ RETURN(tEND_STATIC_OBJECTS);     }
{END_STATIC_OBJS_2}		{ RETURN(tEND_STATIC_OBJECTS);     }

{STATIC_LIB}			{ tlcf_yylval.string = getSection(NULL); RETURN(tSTATIC_LIBRARY);           }
{FOR_STATIC_LIBS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_STATIC_LIBRARIES); }
{FOR_STATIC_LIBS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_STATIC_LIBRARIES); }
{END_STATIC_LIBS_1}		{ RETURN(tEND_STATIC_LIBRARIES);   }
{END_STATIC_LIBS_2}		{ RETURN(tEND_STATIC_LIBRARIES);   }

\<AUTOLOAD\.ID\>		{ RETURN(tAUTOLOAD_ID);            }
\<AUTOLOAD\.NAME\>		{ RETURN(tAUTOLOAD_NAME);          }
\<AUTOLOAD\.ADDRESS\>		{ RETURN(tAUTOLOAD_ADDRESS);       }
\<NUMBER\.AUTOLOADS\>		{ RETURN(tNUMBER_AUTOLOADS);       }

{FOR_AUTOLOADS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_AUTOLOADS); }
{FOR_AUTOLOADS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_AUTOLOADS); }
{END_AUTOLOADS_1}		{ RETURN(tEND_AUTOLOADS);          }
{END_AUTOLOADS_2}		{ RETURN(tEND_AUTOLOADS);          }

{AUTOLOAD_OBJ}			{ tlcf_yylval.string = getSection(NULL); RETURN(tAUTOLOAD_OBJECT);          }
{FOR_AUTOLOAD_OBJS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_AUTOLOAD_OBJECTS); }
{FOR_AUTOLOAD_OBJS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_AUTOLOAD_OBJECTS); }
{END_AUTOLOAD_OBJS_1}		{ RETURN(tEND_AUTOLOAD_OBJECTS);   }
{END_AUTOLOAD_OBJS_2}		{ RETURN(tEND_AUTOLOAD_OBJECTS);   }

{AUTOLOAD_LIB}			{ tlcf_yylval.string = getSection(NULL); RETURN(tAUTOLOAD_LIBRARY);           }
{FOR_AUTOLOAD_LIBS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_AUTOLOAD_LIBRARIES); }
{FOR_AUTOLOAD_LIBS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_AUTOLOAD_LIBRARIES); }
{END_AUTOLOAD_LIBS_1}		{ RETURN(tEND_AUTOLOAD_LIBRARIES); }
{END_AUTOLOAD_LIBS_2}		{ RETURN(tEND_AUTOLOAD_LIBRARIES); }

\<OVERLAY\.ID\>			{ RETURN(tOVERLAY_ID);             }
\<OVERLAY\.NAME\>		{ RETURN(tOVERLAY_NAME);           }
\<OVERLAY\.GROUP\>		{ RETURN(tOVERLAY_GROUP);          }
\<OVERLAY\.ADDRESS\>		{ RETURN(tOVERLAY_ADDRESS);        }
\<NUMBER\.OVERLAYS\>		{ RETURN(tNUMBER_OVERLAYS);        }
\<NUMBER_OVERLAYS\>		{ RETURN(tNUMBER_OVERLAYS);        }

{FOR_OVERLAYS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_OVERLAYS); }
{FOR_OVERLAYS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_OVERLAYS); }
{END_OVERLAYS_1}		{ RETURN(tEND_OVERLAYS);           }
{END_OVERLAYS_2}		{ RETURN(tEND_OVERLAYS);           }

{OVERLAY_OBJ}			{ tlcf_yylval.string = getSection(NULL); RETURN(tOVERLAY_OBJECT);          }
{FOR_OVERLAY_OBJS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_OVERLAY_OBJECTS); }
{FOR_OVERLAY_OBJS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_OVERLAY_OBJECTS); }
{END_OVERLAY_OBJS_1}		{ RETURN(tEND_OVERLAY_OBJECTS);    }
{END_OVERLAY_OBJS_2}		{ RETURN(tEND_OVERLAY_OBJECTS);    }

{OVERLAY_LIB}			{ tlcf_yylval.string = getSection(NULL); RETURN(tOVERLAY_LIBRARY);           }
{FOR_OVERLAY_LIBS_1}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_OVERLAY_LIBRARIES); }
{FOR_OVERLAY_LIBS_2}		{ tlcf_yylval.string = getSection("=*"); RETURN(tFOREACH_OVERLAY_LIBRARIES); }
{END_OVERLAY_LIBS_1}		{ RETURN(tEND_OVERLAY_LIBRARIES);  }
{END_OVERLAY_LIBS_2}		{ RETURN(tEND_OVERLAY_LIBRARIES);  }

\<PROPERTY\.OVERLAYDEFS\>	{ RETURN(tPROPERTY_OVERLAYDEFS);   }
\<PROPERTY\.OVERLAYTABLE\>	{ RETURN(tPROPERTY_OVERLAYTABLE);  }
\<PROPERTY\.SUR?FFIX\>		{ RETURN(tPROPERTY_SUFFIX);        }

[^<\n]*\n?		|
\<			{
	tlcf_yylval.string = strdup(tlcf_yytext);
	RETURN(tSTRING);
}

.   			{ RETURN(tlcf_yytext[0]); }

%%

/*============================================================================
 *  PARSE TLCF FILE
 */
int  ParseTlcfFile( const char* filename )
{
    FILE *fp;
    int   result;
    
    if ( NULL == ( fp = fopen( filename, "r" ) ) )
    {
	fprintf( stderr, "Cannot open %s\n", filename );
	return 2;
    }
    
    tlcf_yyin  = fp;
    result = tlcf_yyparse();
    fclose( fp );

    if ( GetLoopStackLevel( ) > 0 )
    {
	tlcf_yyerror( "Unmatched foreach-end pair" );
	return 1;
    }

    return result ? 1 : 0;
}
