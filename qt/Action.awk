BEGIN {
  print  "struct _Action  {const char* name;int num;};"
  print "static _Action Actions[]={"
    }
$1=="#define" { if(NF==5)  print "{\"" $5 "\"," $3 "},"} 

END { print "{\"NO_ACTION\",0}};"}
