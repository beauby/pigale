BEGIN {
print "#include <TAXI/propdef.h>"
print "#include <TAXI/Tsvector.h>"
print "struct _PropName {"
print "  svector<const char *> name;"
print "  svector<const char *> desc;"
print "  _PropName() : name(0,1023), desc(0,1023)"
print "  {"
print "   for (int i=0; i<1024; i++) desc[i]=name[i]=\"\";"
section=0
}
/@{/ { 
section++
if (section==1) { todo[0]=todo[1]=todo[2]=todo[3]=1}
else if (section==2) todo[1]=todo[2]=todo[3]=0;
else if (section==3) {todo[0]=0; todo[1]=todo[2]=todo[3]=1;}
else if (section==4) {todo[2]=todo[3]=0;}
else if (section==5) {todo[1]=0; todo[2]=1;}
else if (section==6) {todo[2]=0; todo[3]=1;}
}

/#define PROP_[^<]*<[^\\]*\\c/ {
x = gensub("[^<]*<[^\\\\]*\\\\c[ \t]*[^ \t]*[ \t]*","",1)
x = gensub("^[*)]*[ \t]*","",1,x)
for (i=0; i<=3; i++) {
  if (todo[i]==1){
	print "     name[" 256*i "+" $3 "]=\"" $2 "\";"
	if (x!="")
	  print "     desc[" 256*i "+" $3 "]=\"" x "\";"
    }
  }
}
END {
print "  }";
print "};"
print "static _PropName pname;"
print "const char *PropName(int s, int i) { return pname.name[s*256+i];}"
print "const char *PropDesc(int s, int i) { return pname.desc[s*256+i];}"
} 

