#define		HISTORY           10
#define		MAXSYMBOL         31
#define		MAXLINES          20
#define		MOVEWINDOW        0
#define		FLOAT_DEF         "%-6.4f"
#define		DOUBLE_DEF        "%-6.4f"
#define		INT_DEF           "%-8d"
#define		SHORT_DEF         "%-8d"
#define		PROJECT_TITLE    "Dodaam System LTD."

typedef struct symnode {
		int     address;
		char    type;
} sn;

typedef struct listnode {
		int     row;
		int     col;
		struct  listnode	*nxt;
} ln;


