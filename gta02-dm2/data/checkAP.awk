#!/usr/bin/awk -f
BEGIN{
        name="";
        quality=0;
	ap="";
}

{
        if (index($0,"Address:")){
                tmp=substr($0,index($0,":")+1);
                ap=substr(tmp,1,length(tmp));
        }
        if (index($0,"ESSID:")){
                tmp=substr($0,index($0,"\"")+1);
                name=substr(tmp,1,length(tmp)-1);
        }
        if (index($3,"level=")){
                quality=substr($3,8);
		if (name=="56" || name=="78")
	                printf ("%s %s %s\n",quality,name,ap);
                name="";
                quality=0;
		ap="";
        }
}
