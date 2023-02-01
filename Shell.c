#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/resource.h>
#include <stdbool.h>
#define MAX 1024
#define END 4096
#define TAMANO 2048
#define MAXVAR 2048
#define MAXNAME 2048
extern char** environ;
char** envcpy;
void Cmd_autores(char**);
void Cmd_pid(char**);
void Cmd_carpeta(char**);
void Cmd_fin(char**);
void Cmd_comando(char**);
void Cmd_hist(char**);
void ProcesarEntrada (char**);
void Cmd_date (char**);
void Cmd_ayuda(char**);
void Cmd_infosis(char**);
int TrocearCadena(char*,char **);
void Cmd_create (char**);
void Cmd_stat(char**);
void Cmd_list(char**);
void Cmd_delete(char**);
void Cmd_deltree(char**);
void Cmd_Memory(char**);
void Cmd_allocate(char**);
void Cmd_deallocate(char**);
void Cmd_Recursive(char**);
void Cmd_MemFill (char**);
void Cmd_io (char**);
void Cmd_Memdump (char**);
void Cmd_showvar (char**);
void Cmd_showenv(char**);
void Cmd_priority (char**);
void Cmd_Changevar(char**);
void Cmd_fork(char**);
void Cmd_execute (char**);
void Cmd_listjobs (char**);
void Cmd_deljobs (char**);
void Cmd_job(char**);
struct CMD
{
    char *name;
    void (*func)(char**);
};
struct CMD c[] = {
        {"autores", Cmd_autores},
        {"carpeta", Cmd_carpeta},
        {"pid", Cmd_pid},
        {"fin", Cmd_fin},
        {"bye", Cmd_fin},
        {"salir", Cmd_fin},
        {"exit", Cmd_fin},
        {"comando", Cmd_comando},
        {"hist", Cmd_hist},
        {"fecha", Cmd_date},
        {"ayuda", Cmd_ayuda},
        {"help", Cmd_ayuda},
        {"infosis",Cmd_infosis},
        {"create",Cmd_create},
        {"stat",Cmd_stat},
        {"list",Cmd_list},
        {"delete", Cmd_delete},
        {"deltree", Cmd_deltree},
        {"allocate", Cmd_allocate},
        {"deallocate", Cmd_deallocate},
        {"recurse", Cmd_Recursive},
        {"memfill", Cmd_MemFill},
        {"memory", Cmd_Memory},
        {"i-o",Cmd_io},
        {"memdump", Cmd_Memdump},
        {"showvar",Cmd_showvar},
        {"showenv",Cmd_showenv},
        {"priority",Cmd_priority},
        {"changevar",Cmd_Changevar},
        {"fork",Cmd_fork},
        {"execute",Cmd_execute},
        {"listjobs",Cmd_listjobs},
        {"deljobs",Cmd_deljobs},
        {"job",Cmd_job},
        { NULL, NULL }
};
//AUXILIARY FUNCTIONS AND GLOBAL DECLARATION OF LISTS
//SOME OF THE FUNCTIONS ARE OURS, SOME OTHERS PROVIDED BY PROFESSORS

//PRACTICE 0--------------------------------------------------------------
//historic
char *hist[END];
int limit = 0;
void InsertCommand(char *entrada){
    hist[limit] = malloc(MAX);
    strcpy(hist[limit], entrada);
    limit++;
    hist[limit] = NULL;
}
void clearhist(char* hist[]){
    int i=0;
    while(i<limit){
        free(hist[i]);
        i++;
    }
    limit=0;
}
//PRACTICE 1--------------------------------------------------------------
char LetraTF (mode_t m)
{
    switch (m&S_IFMT) {
        case S_IFSOCK: return 's';
        case S_IFLNK: return 'l';
        case S_IFREG: return '-';
        case S_IFBLK: return 'b';
        case S_IFDIR: return 'd';
        case S_IFCHR: return 'c';
        case S_IFIFO: return 'p';
        default: return '?';
    }
}
char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    return permisos;
}
char* userNameFromId(uid_t uid)
{
    struct passwd *pwd;
    pwd = getpwuid(uid);
    return (pwd == NULL) ? NULL : pwd->pw_name;
}
char * groupNameFromId(gid_t gid)
{
    struct group *grp;
    grp = getgrgid(gid);
    return (grp == NULL) ? NULL : grp->gr_name;
}
void DoStat(char* path, int lon, int link, int acc) {
    struct stat sb;
    char linked[MAX]="";
    char time[20];
    lstat(path, &sb);
    strftime(time, 20, "%Y/%m/%d-%H:%M", localtime(&sb.st_mtime));
    if(lon==0){
        printf("%d %s\n", (int)sb.st_size, path);
        return;
    }
    if (link == 1){
        if(S_ISLNK(sb.st_mode)){
            char buf[MAX];
            int n =readlink(path, buf, MAX);
            buf[n] = '\0';
            strcpy(linked,"-> ");
            strcat(linked,buf);
        }
    }
    if (acc == 1){
        strftime(time, 20, "%Y/%m/%d-%H:%M", localtime(&sb.st_atime));
    }
    if (lon == 1)
        printf("%s %d (%8d) %s %s %s %d %4s %s\n", time, (int) sb.st_nlink, (int) sb.st_ino, userNameFromId(sb.st_uid),groupNameFromId(sb.st_gid), ConvierteModo2(sb.st_mode), (int) sb.st_size, basename(path), linked);
}
void ListDir(char* initFile, int hidden,int lon,int link,int acc) {
    struct stat sb;
    struct dirent *dir;
    char path[2048];
    DIR *d = opendir(initFile);
    if (d) {
        printf("*************%s\n", initFile);
        while ((dir = readdir(d)) != NULL) {
            sprintf(path, "%s/%s", initFile, dir -> d_name);
            lstat(path, &sb);
            if(lon == 0){
                if (hidden==1)
                    printf("%d %s\n", (int) sb.st_size, dir->d_name);
                else if (dir->d_name[0] != '.')
                    printf("%d %s\n", (int) sb.st_size, dir->d_name);
            }
            else{
                if (hidden==1)
                    DoStat(path, lon, link, acc);
                else if (dir->d_name[0] != '.')
                    DoStat(path,lon,link,acc);
            }
        }
        closedir(d);
    }
}
void ListRec(char *initFile, int hidden, int i, int lon, int link, int acc){
    struct dirent *dir = NULL;
    char path [2048];
    DIR *d = NULL;
    if ((d = opendir(initFile)) == NULL){
        return;
    }
    while ((dir = readdir(d)) != NULL) {
        if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")){
            continue;
        }
        else sprintf(path, "%s/%s", initFile, dir-> d_name);
        if(i == 1)
            ListRec(path, hidden, i, lon, link, acc);
        ListDir(path, hidden, lon, link, acc);
        if(i == 2)
            ListRec(path, hidden, i, lon, link, acc);
    }
    closedir(d);
}
void delete (char* dirc){
    DIR* d;
    struct dirent *dir;
    struct stat sb;
    if(lstat(dirc, &sb)==-1){
        perror("Error");
        return;
    }
    if (S_ISDIR(sb.st_mode)){
        if ((d = opendir(dirc))==NULL){
            perror("Error");
            return;
        }
        while ((dir = readdir(d)) != NULL ) {
            char path[2048];
            if (strcmp(dir->d_name,"..")==0 || strcmp(dir->d_name,".")==0){continue;}
            sprintf (path, "%s/%s",dirc,dir->d_name);
            delete(path);
        }
        closedir(d);
    }
    if (remove(dirc)==-1){
        perror("Error");
    }
}
//PRACTICE  2--------------------------------------------------------------
//blocklist
int lastblock = 0;
struct block{
    void* memaddress;
    int size;
    char alloctime[40];
    enum class {mallocs, shared , mapped} type;
    //-------------- shared
    int key;
    //-------------- mapped
    char name[MAX];
    int descriptor;
};
struct block* BlockList[END];
void InsertBlock(struct block* structlist[],void *p, size_t size, key_t clave, int df, char fichero[]){
    struct block *thisblock=malloc(sizeof(struct block));
    char buf[80];
    time_t result = time(NULL);
    strftime(buf, 20, "%Y/%m/%d-%H:%M", localtime(&result));
    thisblock->memaddress = p;
    thisblock->size = (int)size;
    strcpy(thisblock->alloctime,buf);
    if(clave != 0){
        thisblock->type = shared;
        thisblock->key = clave;
    }
    else if (df != 0 && fichero!=NULL){
        thisblock->type = mapped;
        thisblock->descriptor = df;
        strcpy(thisblock->name,fichero);
    }
    else thisblock->type = mallocs;

    structlist[lastblock] = thisblock;
    lastblock++;
    structlist[lastblock] = NULL;
}
struct block* blockfromaddress(void* p){
    int i = 0;
    while(BlockList[i] != NULL){
        if(BlockList[i]->memaddress == p) return BlockList[i];
        else i++;
    }
    return NULL;
}
char* enumToCharAlloc(enum class type) {
    switch (type) {
        case mallocs :
            return "malloc";
        case shared :
            return "shared";
        case mapped :
            return "mapped";

        default:
            return "xd";
    }
}
struct block* getblock(int key,char* file,int size){
    if(key != 0){
        int i = 0;
        while(BlockList[i] != NULL){
            if(BlockList[i]->key == key){
                return BlockList[i];
            }
            else i++;
        }
    }
    else if (file != NULL){
        int i = 0;
        while(BlockList[i] != NULL){
            if(!strcmp(BlockList[i]->name, file)){
                return BlockList[i];
            }
            else i++;
        }
    }
    else if (size != 0){
        int i = 0;
        while(BlockList[i] != NULL){
            if(BlockList[i]->size == size){
                return BlockList[i];
            }
            i++;
        }
    }
    return NULL;
}
void deleteSingleBlock(struct block* structlist[], void *p){
    int i = 0;
    lastblock--;
    while(structlist[i] != NULL){
        if(structlist[i]->memaddress== p){
            free(structlist[i]);
            break;
        }
        i++;
    }
    for (int j = i; j<=lastblock; j++) {
        structlist[i] = structlist[i + 1];
    }
}
void printBlockList(char* type, struct block* list[]){
    int i=0;
    printf("*** Blocklist assigned to process %d\n", getpid());
    while(i<lastblock){
        if(type == NULL){
            printf("%p%16d%19s    %s", list[i]->memaddress, list[i]->size, list[i]->alloctime,
                   enumToCharAlloc(list[i]->type));
            if(list[i]->type == mallocs) printf("\n");
            else if(list[i]->type == shared )
                printf("   (key: %d)\n",list[i]->key);
            else if (list[i]->type == mapped)
                printf("       %s    (descriptor %d)\n",list[i]->name, list[i]->descriptor);
        }
        else if((!strcmp("-malloc",type)) && list[i]->type == mallocs) {
            printf("%p%16d%19s    %s\n", list[i]->memaddress, list[i]->size, list[i]->alloctime,
                   enumToCharAlloc(list[i]->type));
        }
        else if((!strcmp("-shared",type) || !strcmp("-createshared",type)) && list[i]->type == shared){
            printf("%p%16d%19s    %s", list[i]->memaddress, list[i]->size, list[i]->alloctime,
                   enumToCharAlloc(list[i]->type));
            printf("   (key: %d)\n",list[i]->key);
        }
        else if((!strcmp("-mmap",type)) && list[i]->type == mapped){
            printf("%p%16d%19s    %s", list[i]->memaddress, list[i]->size, list[i]->alloctime,
                   enumToCharAlloc(list[i]->type));
            printf("       %s    (descriptor %d)\n",list[i]->name, list[i]->descriptor);
        }
        i++;
    }
}
int isNumber(char s[])
{
    int j = atoi (s);
    if (isdigit(j) != 0)
        return 1;
    return 0;
}
void* cadtop (char *tr){
    void * re;
    sscanf(tr,"%p",&re);
    return re;
}
ssize_t LeerFichero (char *f, void *p, size_t cont){
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}
ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite){
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;

    if ((df=open(f,flags,0777))==-1)
        return -1;

    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}
void * ObtenerMemoriaShmget (key_t clave, size_t tam){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  /*no nos vale*/
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    InsertBlock(BlockList, p, s.shm_segsz, clave, 0, NULL);
    return (p);
}
void * MapearFichero (char * fichero, int protection){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1){
        return NULL;
    }
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED){
        return NULL;
    }
    InsertBlock(BlockList, p, s.st_size, 0, df, fichero);
    return p;
}
void do_AllocateCreateshared (char *tr[]){
    key_t cl;
    size_t tam;
    void *p;
    cl=(key_t)  strtoul(tr[0],NULL,10);
    tam=(size_t) strtoul(tr[1],NULL,10);
    if (tam==0) {
        printf ("0-byte blocks are not assignable\n");
        return;
    }
    if ((p=ObtenerMemoriaShmget(cl,tam))!=NULL)
        printf (" %lu bytes assigned at %p\n",(unsigned long) tam, p);
    else
        printf ("Impossible to assign shared memory with key %lu:%s\n",(unsigned long) cl,strerror(errno));
}
void do_AllocateMmap(char *arg[]){
    char *perm;
    void *p;
    int protection=0;
    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    else protection=PROT_READ|PROT_WRITE|PROT_EXEC;
    if ((p=MapearFichero(arg[0],protection))==NULL)
        perror ("File impossibleto map");
    else
        printf ("file %s mapped at %p\n", arg[0], p);
}
void dealmalloc(void* p){
    printf ("%d bytes deallocated at %p\n",blockfromaddress(p)->size,p);
    deleteSingleBlock(BlockList, p);
    free(p);
}
void dealshared(void *p){
    if (shmdt(p)==-1){
        perror("Error: xd");
        return;
    }
    printf ("Memory %p with key %d deallocated\n", p,blockfromaddress(p)->key);
    deleteSingleBlock(BlockList, p);
}
void dealmap(void* p){
    if (munmap(p, blockfromaddress(p)->size) == -1) {
        perror("Error");
    }
    printf ("file %s unmapped from %p\n", blockfromaddress(p)->name, p);
    deleteSingleBlock(BlockList, p);
}
void clearBlockList(struct block* structlist[]){
    int i = 0;
    while(i<lastblock){
        if(structlist[i]->type == mallocs) dealmalloc(structlist[i]->memaddress);
        if(structlist[i]->type == shared) dealshared(structlist[i]->memaddress);
        if(structlist[i]->type == mapped) dealmap(structlist[i]->memaddress);
    }
}
void do_DeallocateDelkey (char *args[]){
    key_t clave;
    int id;
    char *key=args[0];
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey needs valid key\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible to obtain shared memory");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible to delete shared memory\n");
    else printf ("key %s deallocated\n", args[0]);

}
void do_I_O_read (char *ar[]){
    void *p;
    size_t cont=-1;
    ssize_t n;
    if (ar[1]==NULL || ar[2]==NULL){
        printf ("faltan parametros\n");
        return;
    }
    p=cadtop(ar[2]);  /*convertimos de cadena a puntero*/
    if (ar[3]!=NULL)
        cont=(size_t) atoll(ar[3]);

    if ((n=LeerFichero(ar[1],p,cont))==-1)
        perror ("Imposible leer fichero");
    else
        printf ("leidos %lld bytes de %s en %p\n",(long long) n,ar[1],p);
}
void do_I_O_write (char *ar[]){
    void *p;
    size_t cont=-1;
    ssize_t n;
    int x=0;
    if (strcmp(ar[1],"-o")==0){
        x++;
    }
    if (ar[1+x]==NULL || ar[2+x]==NULL){
        printf ("Some parameters are left\n");
        return;
    }
    p=cadtop(ar[2+x]);
    if (ar[3+x]!=NULL)
        cont=(size_t) atoll(ar[3+x]);

    if ((n=EscribirFichero(ar[1+x],p,cont,x))==-1)
        perror ("File impossible to read");
    else
        printf ("%lld bytes written in %s at %p\n",(long long) n,ar[1+x],p);
}
void Do_pmap (void){
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};
    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Process imposible to create");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");
        exit(1);
    }
    waitpid (pid,NULL,0);
}
void printfuncs(){
    printf("Program functions:\t%p, %p, %p\n", Cmd_date, Cmd_pid, Cmd_list);
    printf("Library functions:\t%p, %p, %p\n", printf, perror, strcpy);
}
void printvars(){
    int a1=0,a2=0,a3=0;
    static int s1=0,s2=0,s3=0;

    printf("Local variables:\t%p, %p, %p\n", &a1, &a2, &a3);
    printf("Static variables:\t%p, %p, %p\n", &s1, &s2, &s3);
    printf("Global variables:\t%p, %p, %p\n", &lastblock, &limit, &envcpy);
}
void Recursiva (int n)
{
    char automatico[TAMANO];
    static char estatico[TAMANO];
    printf ("parameter:%3d(%p) array %p, static array %p\n",n,&n,automatico, estatico);
    if (n>0)
        Recursiva(n-1);
}
void LlenarMemoria (void *p, size_t cont, unsigned char byte){
    unsigned char *arr=(unsigned char *) p;
    size_t i;
    for (i=0; i<cont;i++)
        arr[i]=byte;
}
void ShowMemory(void *p, size_t cont) {
    char *byte = p;
    int xd =(int) cont;
    for (int k = 0; k < (cont / 25)+1 ; k++) {
        for (int i = 0; i < 25 && i<xd; i++) {
            printf("%c  ", byte[i]);
        }
        if (xd!=0) printf("\n");
        for (int i = 0; i < 25&& i<xd; i++) {
            printf("%02x ", byte[i]);
        }
        if(xd!=0)printf("\n");
        xd-=25;
    }
}
//PRACTICE 3--------------------------------------------------------------
//processlist
int lastprocess = 0;
struct process{
    int pid;
    char launchtime[40];
    int signal;
    char cmd[MAX];
    int priority;
    enum stext {
        FINISHED,
        STOPPED,
        ACTIVE,
        SIGNALED
    }stext;
};
struct process* processlist[MAX];
struct process* getProcess(int pid){
    int i = 0;
    while(processlist[i]!=NULL){
        if(processlist[i]->pid==pid) return processlist[i];
        ++i;
    }
    return NULL;
}
char* enumToCharProc(enum stext stext){
    switch (stext){
        case FINISHED : return "FINISHED";
        case STOPPED : return "STOPPED";
        case ACTIVE : return "ACTIVE";
        case SIGNALED : return "SIGNALED";

        default: return "xd";
    }
}
void modProc(struct process* proc, enum stext status , int signal) {
    proc->stext=status;
    proc->signal=signal;
}
void updateProcessStatus(struct process** proc) {
    int status;
    if (waitpid ((*proc)->pid,&status, WNOHANG | WUNTRACED | WCONTINUED) == (*proc)->pid) {
        if (WIFEXITED(status)) {
            modProc(*proc, FINISHED, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            modProc(*proc, SIGNALED, WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            modProc(*proc, STOPPED, WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            modProc(*proc, ACTIVE, -1);
        }
    }
}
void insertProcess(struct process* processlist[], int pid,char* cmd,int priority){
    struct process* proc = malloc(sizeof(struct process));
    char buf[80];
    time_t result = time(NULL);
    strftime(buf, 20, "%Y/%m/%d-%H:%M", localtime(&result));

    strcpy(proc->launchtime, buf);
    proc->pid = pid;
    proc->stext= ACTIVE;

    strcpy(proc->cmd,cmd);
    proc->priority=priority;
    proc->signal = 0;

    processlist[lastprocess] = proc;
    lastprocess++;
    processlist[lastprocess] = NULL;
}
void deleteSingleProcess(int pid){
    int i=0;
    while(i<lastprocess){
        if(processlist[i]->pid == pid) {
            free(processlist[i]);
            for (int j = i; j <= lastblock; j++) {
                processlist[i] = processlist[i + 1];
            }
            lastprocess--;
            processlist[lastprocess]=NULL;
            return;
        }
        i++;
    }
}
void deleteProcesses(enum stext stext){
    int i=0;
    while(i<lastprocess){
        if(processlist[i]->stext == stext) {
            deleteSingleProcess(processlist[i]->pid);
        }
        i++;
    }
}
void printJobinfo(struct process* proc) {
    updateProcessStatus(&proc);
    printf("%d    %s   p=%d   %s  %s  (%d) %s\n", proc->pid,
           userNameFromId(getuid()) ,getpriority(PRIO_PROCESS, proc->pid), proc->launchtime, enumToCharProc(proc->stext),proc->signal,
           proc->cmd);
}
void printJobs(){
    if(processlist[0]==NULL) return;
    int i = 0;
    while(processlist[i]!=NULL){
        printJobinfo(processlist[i]);
        ++i;
    }
}
void ClearPL(struct process* processlist[]){
    if(processlist[0]==NULL) return;
    int i=0;
    while(i<=lastprocess){
        deleteSingleProcess(processlist[0]->pid);
        i++;
    }
    lastprocess=0;
}
void updatePrio(int pid, int prio){
    struct process* proc;
    if ((proc = getProcess(pid))!=NULL){
        proc->priority = prio;
    }
}
void joinString(char* tr[]){
    int i = 1;
    while(tr[i]!=NULL){
        if (tr[i][0]=='&'){ tr[i]=NULL; break;}
        strcat(tr[0]," ");
        strcat(tr[0],tr[i]);
        ++i;
    }
}
bool checkBack(char* tr[]){
    int i = 0;
    while(tr[i]!=NULL){
        if(!strcmp(&tr[i][0],"&")){
            return true;
        }
        ++i;
    }
    return false;
}
int BuscarVariable (char * var, char* e[]) 
{
    int pos=0;
    char aux[MAXVAR];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else
            pos++;
    errno=ENOENT;   
    return(-1);
}

int ChangeVariable(char * var, char * valor, char *e[])
{
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}
void MostrarEntorno (char **entorno, char *nombre_entorno){
    int i=0;
    while (entorno[i]!=NULL) {
        printf ("%p->%s[%d]=(%p) %s\n", &entorno[i],nombre_entorno, i,entorno[i],entorno[i]);
        i++;
    }
}
char * Ejecutable (char *s)
{
    char path[MAXNAME];
    static char aux2[MAXNAME];
    struct stat st;
    char *p;
    if (s==NULL || (p=getenv("PATH"))==NULL)
        return s;
    if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;       
    strncpy (path, p, MAXNAME);
    for (p=strtok(path,":"); p!=NULL; p=strtok(NULL,":")){
        sprintf (aux2,"%s/%s",p,s);
        if (lstat(aux2,&st)!=-1)
            return aux2;
    }
    return s;
}
int OurExecvpe(char *file, char *const argv[], char *const envp[])
{
    return (execve(Ejecutable(file),argv, envp));
}
void getArguments(char *tr[], char *output[], char* envvar[]) {
    int i = 0, j=0;
    while (getenv(tr[i])!=NULL){
        envvar[i]=environ[BuscarVariable(tr[i], environ)];
        i++;
    }
    envvar[i]=NULL;
    while (tr[i] != NULL) {
        if (tr[i][0] == '@' || tr[i][0]=='&')
            break;
        output[j] = tr[i];
        i++;
        j++;
    }
    output[j]=NULL;
}
//COMMANDS---------------------------------------------------------------
void Cmd_ayuda(char *tr[]){
    if(tr[0]==NULL){
        printf("autores pid carpeta fecha hist comando infosis ayuda bye create stat list delete deltree allocate dealloacate i-o memdump memfill memory recurse priority showvar changevar showenv fork execute listjobs deljobs job\n");
        return;
    }
    if(!strcmp(tr[0],"autores")){
        printf("This command prints the names and the logins of the authors of the shell which you are using\n"
               "authors -l prints only the logins and authors -n prints only the names\n");}
    else if(!strcmp(tr[0],"pid")){
        printf("This command prints the pid of the process executing the shell.\n"
               "pid -p prints the pid of the shell’s parent process.\n");}
    else if(!strcmp(tr[0],"carpeta")){
        printf("This command changes the current working directory of the shell to direct (using the chdir system call).\n"
               "When invoked without arguments it prints the current working directory \n");}
    else if(!strcmp(tr[0],"fecha"))
        printf("This command ,without arguments, prints both the current date and the current time.\n"
               "fecha -d prints the current date in the format DD/MM/YYYY.fecha -h prints the current time in the format hh:mm:ss.\n");
    else if(!strcmp(tr[0],"hist"))
        printf("This command shows/clears the historic of commands executed by this shell\n"
               "hist Prints all the commands with their order number\n"
               "hist -c empties the historic of commands\n"
               "hist -N prints the first N commands\n");
    else if(!strcmp(tr[0],"comando"))
        printf("This command repeats command number N (from historic list)\n");
    else if(!strcmp(tr[0],"infosis"))
        printf("This command prints information on the machine running the shell\n");
    else if(!strcmp(tr[0],"ayuda"))
        printf("This command displays a list of available commands.\n");
    else if(!strcmp(tr[0],"bye")||!strcmp(tr[0],"fin")|| !strcmp(tr[0],"salir") || !strcmp(tr[0],"exit"))
        printf("This command ends the shell\n");
    else if (!strcmp(tr[0],"create")){
        printf("This command, without arguments, shows you the directory you are in.\n"
               "With arguments it creates a directory with the same name as the argument.\n"
               "If you add '-f' before the argument, it creates a file instead.\n");
    }
    else if (!strcmp(tr[0],"stat")){
        printf("This command without arguments, shows you the directory you are in.\n"
               "With arguments, it shows the size of the the file input, next to its name.\n"
               "If you add '-long', it also shows you the last time it was modified, the number of inodes, the file serial number, the owner and its group and the permissions.\n"
               "If you add '-acc' it shows you the last time it was modified.\n"
               "If you add '-link' and the input is a symbolic link, it shows you its path.");
    }
    else if (!strcmp(tr[0],"list")){
        printf ("This command without arguments, shows you the directory you are in.\n"
                "If you put a directory name inside of where you are, it prints its content.\n"
                "If you add '-hid', it also shows the hidden files."
                "If you add '-reca' it shows the files and then the content of the directories in it.\n"
                "If you add 'recb' it shows the content of the directories inside the input, and then the files of it."
                "You can also add '-long' '-acc' and '-link' which print the same as the 'stat' command\n.");
    }
    else if (!strcmp(tr[0],"delete")){
        printf("This command without arguments, shows you the directory you are in.\n"
               "If you put file names or an empty directory names, it deletes them. It accepts as much as you want.\n");
    }
    else if (!strcmp(tr[0],"deltree")){
        printf("This command without arguments, shows you the directory you are in.\n"
               "If you put file names or an directory names, it deletes them. It accepts as much as you want. The content of the directory is also deleted\n");
    }
    else if(!strcmp(tr[0],"allocate")){
        printf("allocate [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria\n"
	"-malloc tam: Allocates a block malloc o size tam\n"
	"-createshared cl tam: Allocates (creating) the shared memory block with key cl and size tam\n"
	"-shared cl: Allocates the shared memory block (already existing) of key cl\n"
	"-mmap fich perm: Maps the file fich, perm are the permissions\n");  
    } 
    else if(!strcmp(tr[0],"deallocate")){
        printf("deallocate [-malloc|-shared|-delkey|-mmap|addr]..	Deallocates a memory block\n"
	"-malloc siz: Deallocates the block malloc of size siz\n"
	"-shared cl: Deallocates (demaps) the memory block shared of key cl\n"
	"-delkey cl: Removes from the system (without demmapping) the memory key cl\n"
	"-mmap fich: Demaps the mapped file fich\n"
	"addr: Deallocates the memory block from the address addr\n");
    } 
    else if(!strcmp(tr[0],"i-o")){
        printf("i-o [read|write] [-o] fiche addr cont\n"
	"read fich addr cont: Reads cont bytes from fich to addr\n"
	"write [-o] fich addr cont: Writes cont bytes since addr to fich. -o to overwrite\n"
		"addr is a memory address\n");
    } 
    else if(!strcmp(tr[0],"memdump")){
        printf("memdump addr cont 	Dumps in screen the contents (cont bytes) of the position of memory addr\n");
	} 
    else if(!strcmp(tr[0],"memfill")){
        printf("memfill addr cont byte 	Fills the memory since addr with byte\n");
    } 
    else if(!strcmp(tr[0],"memory")){
        printf("memory [-blocks|-funcs|-vars|-all|-pmap] ..	Shows details about the process memory\n"
		"-blocks: the memory blocks assigned\n"
		"-funcs: the function addresses\n"
		"-vars: the variable addresses\n"
		":-all: everything\n"
		"-pmap: shows the output of the coomand pmap (or similar)\n");
    } 
    else if (!strcmp(tr[0], "recurse")){
    	printf("recurse [n]	Invokes the recursive function n times\n");
    	}
    	
    else if(!strcmp(tr[0],"priority")){
    printf("priority [pid] [value] 	Shows or changes the priority of process pid to value\n");
    } 
    else if(!strcmp(tr[0],"showvar")){
    printf("showvar var1	Shows the value and addresses of the environment variable var\n"); 
    } 
    else if(!strcmp(tr[0],"changevar")){
    printf ("changevar [-a|-e|-p] var value	Changes the value of an environment variable\n"
	"-a: accesses through main arg3\n"
	"-e: accesses through environ\n"
	"-p: accesses through putenv\n");
    } 
    else if (!strcmp(tr[0], "showenv")){
    	printf("showenv [-environ|-addr] 	 It shows the environment process\n");
    }
    else if (!strcmp(tr[0], "fork")){
    	printf("fork 	It makes a fork call in order to create a process\n");
    }
    else if (!strcmp(tr[0], "listjobs")){
    	printf("listjobs 	Lists the background processes\n");
    }
    else if (!strcmp(tr[0], "execute")){
    	printf("execute prog args....	Executes,without creating any processes, prog with arguments\n");
    }
    else if (!strcmp(tr[0], "deljobs")){
    	printf("deljobs [-term][-sig]	Removes the processes from the processlist in sp\n"
	"-term: the finished ones\n"
		"-sig: the ones finished by signal\n");
    }
    else if (!strcmp(tr[0], "job")){
    	printf("job [-fg] pid	Shows information about the process pid.\n"
		"-fg: it sends it to the foreground\n");
    }
    else perror("Error: command not found\n");
}
void Cmd_infosis(char *tr[])
{
    if(tr[0]==NULL){
        struct utsname unameData;
        uname(&unameData);
        printf("System name:  %s\n", unameData.sysname);
        printf("Node name: %s\n", unameData.nodename);
        printf("Release:  %s\n", unameData.release);
        printf("Version:  %s\n", unameData.version);
        printf("Machine:  %s\n", unameData.machine);
        return;
    }
    else{
        printf("Error: command not found\n");
        return;
    }
}
void Cmd_autores(char *tr[])
{
    if(tr[0]==NULL){
        printf("logins: s.puertas@udc.es\txabier.x.carricoba@udc.es.\n");
        printf("names: Sergio Puertas Pérez\tXabier Xoán Carricoba Muñoa.\n");
        return;
    }
    if(!strcmp(tr[0],"-l"))
        printf("logins: s.puertas@udc.es\txabier.x.carricoba@udc.es.\n");
    else if(!strcmp(tr[0],"-n"))
        printf("names: Sergio Puertas Pérez\tXabier Xoán Carricoba Muñoa.\n");
    else{
        printf("Error: command not found\n");
    }
}
void Cmd_pid(char *tr[])
{
    if(tr[0]!=NULL && !strcmp(tr[0],"-p")){
        printf("Parent process pid: %d\n",getpid());
    }
    else if (tr[0]==NULL)
        printf("Process pid %d\n",getpid());
    else
        printf("Error: Command not found\n");
}
void Cmd_carpeta(char *tr[])
{
    char dir[MAX];
    if(tr[0]==NULL)
        printf("%s\n",getcwd(dir,MAX));
    else if (chdir(tr[0])==-1){
        perror("Error");
    }
    else if (chdir(tr[0])==0){}
    else printf("Error: command not found\n");
}
void Cmd_comando(char *tr[]) {
    char *trozos[MAX], entrada[MAX];
    static int bucle = -1;
    if (tr[0] == NULL){
        printf("Error: argument is required\n");
        return;
    }
    if(atoi(tr[0]) < 0 || atoi(tr[0]) >=limit - 1){
        printf("There is no element %d in the historic\n",atoi(tr[0]));
        return;
    }
    else if (atoi(tr[0]) <= limit && limit != 0) {
        strcpy(entrada, hist[atoi(tr[0])]);
        if (TrocearCadena(entrada, trozos) == 0) {}
        if (!strcmp(trozos[0], "comando")) {
            if (bucle == atoi(trozos[1])) {
                printf("Error: command 'Comando' has entered an infinite loop, leaving the iteration...\n");
                return;
            }
            bucle = atoi(trozos[1]);
        }
        ProcesarEntrada(trozos);
        return;
    }
}
void Cmd_hist(char *tr[])
{
    int i;
    if(tr[0]==NULL || !strcmp(tr[0],"0")){
        for(i = 0; i < limit; i++){
            printf("%d-> %s",i,hist[i]);
        }
        return;
    }
    else if(!strcmp(tr[0],"-c")) {
        clearhist(hist);
        return;
    }
    else if(atoi(tr[0]) < -limit){
        printf("There is no element %d in the historic\n",-atoi(tr[0]));
        return;
    }
    else if(atoi(tr[0])<0) {
        for (i = 0; i >= atoi(tr[0]); i--) {
            printf("%d-> %s", -i, hist[-i]);
        }
        return;
    }
    else if(!strcmp(tr[0],"-0")) {
        printf("%d-> %s", 0, hist[0]);
        return;
    }
    else printf("Error: Command not found\n");
}
void Cmd_date (char *tr[])
{
    char tiempo[80];
    if(tr[0]==NULL){
        time_t result = time(NULL);
        strftime( tiempo, 40, "%d/%m/%Y\n%T\n", localtime(&result));
        printf("%s", tiempo);
        return;
    }
    if (!strcmp(tr[0],"-d")){
        time_t result = time(NULL);
        strftime( tiempo, 40, "%d/%m/%Y\n", localtime(&result));
        printf("%s", tiempo);
        return;}
    else if (!strcmp(tr[0],"-h")){
        time_t result = time(NULL);
        strftime( tiempo, 40, "%T\n", localtime(&result));
        printf("%s", tiempo);
        return;}
    else perror("Error: Command not found\n");
}
void Cmd_fin(char* tr[])
{
    clearhist(hist);
    clearBlockList(BlockList);
    exit(0);
}

void Cmd_create (char *tr[])
{
    char dir[MAX];
    if(tr[0]==NULL || (!strcmp(tr[0],"-f") && tr[1]==NULL)){
        printf("%s\n",getcwd(dir,MAX));
        return;
    }
    if (fopen(tr[1],"r")!=NULL || ((fopen(tr[0],"r")!=NULL) && (strcmp(tr[0],"-f"))!=0)){
        printf("Error: File already exists\n");
        return;
    }
    if (!strcmp(tr[0],"-f") && tr[1]!=NULL){
        FILE * fp;
        fp = fopen (tr[1],"w+");
        fclose(fp);
        return;
    }
    else
    {
        char newDir[2048];
        getcwd(dir,MAX);
        sprintf(newDir, "%s/%s", dir, tr[0]);
        mkdir(newDir,S_IRWXO|S_IRWXU|S_IRWXG);
    }
}
void Cmd_stat(char *tr[]){
    char dir[MAX];

    if(tr[0]==NULL){
        printf("%s\n",getcwd(dir,MAX));
        return;
    }
    int lon, link, acc;
    lon= link= acc=0;
    for (int i = 0; tr[i] !=NULL ; ++i) {
        if(!strcmp(tr[i],"-long")) lon = 1;
        else if(!strcmp(tr[i],"-link")) link = 1;
        else if(!strcmp(tr[i],"-acc")) acc = 1;
        else break;
    }
    if((tr[link+lon+acc])==NULL){
        printf("%s\n",getcwd(dir,MAX));
        return;
    }
    if(fopen(tr[link+lon+acc],"r")==NULL){
        printf("Error acceding %s:\n",tr[link+lon+acc]);
        perror("");
        return;
    }
    for(int i = link+lon+acc; tr[i] !=NULL ; i++){
        strcpy(dir,tr[i]);
        DoStat(dir,lon,link,acc);
    }
}
void Cmd_list(char *tr[]) {
    char dir[MAX];
    struct stat sb;
    int  reca, recb, lon, link, acc, hid;
    hid = reca = recb = lon = link = acc = 0;
    if (tr[0] == NULL) {
        printf("%s\n", getcwd(dir, MAX));
        return;
    }
    for (int i = 0; tr[i] != NULL; ++i) {
        if (!strcmp(tr[i], "-hid")) hid = 1;
        else if (!strcmp(tr[i], "-reca")) reca = 1;
        else if (!strcmp(tr[i], "-recb")) recb = 1;
        else if (!strcmp(tr[i], "-long")) lon = 1;
        else if (!strcmp(tr[i], "-link")) link = 1;
        else if (!strcmp(tr[i], "-acc")) acc = 1;
    }
    if ((tr[hid + reca + recb + lon + link + acc]) == NULL) {
        printf("%s\n", getcwd(dir, MAX));
        return;
    }
    for (int i = hid + reca + recb + lon + link + acc; tr[i] != NULL; i++) {
        lstat(tr[i], &sb);
        if (opendir(tr[hid + reca + recb + lon + link + acc]) == NULL) {
            perror("");
            return;
        }
        if(!S_ISDIR(sb.st_mode)){
            DoStat(tr[i],lon,link,acc);
            return;
        }
        if (recb == 1) {
            ListRec(tr[i], hid, 1, lon, link, acc);
        }
        ListDir(tr[i], hid, lon, link, acc);
        if (reca == 1) {
            if (recb == 1) return;
            ListRec(tr[i], hid, 2, lon, link, acc);
        }
    }
}
void Cmd_delete(char* tr[]) {
    struct stat sb;
    char dir[MAX] ;
    if (tr[0]==NULL){
        printf("%s\n",getcwd(dir,MAX));
        return;
    }
    if (!strcmp(tr[0],".") || !strcmp(tr[0],"..")){
        printf("Imposible borrar %s: Invalid argument\n", tr[0]);
    }
    for (int i = 0; tr[i] != NULL; i++) {
        if ((lstat(tr[i], &sb) == -1)||(S_ISREG(sb.st_mode) && remove(tr[i]) == -1)||(S_ISDIR(sb.st_mode) && (rmdir(realpath(tr[i], 0)) == -1))) {
            printf("Imposible borrar %s\n",tr[i]);
            perror("");
        }
    }
}
void Cmd_deltree(char* tr[]){
    char dir[MAX];
    if (tr[0]==NULL){
        printf("%s\n",getcwd(dir,MAX));
        return;
    }
    if (!strcmp(tr[0],".") || !strcmp(tr[0],"..")){
        printf("Imposible borrar %s: Invalid argument\n", tr[0]);
    }
    for (int i = 0; tr[i] != NULL; i++) {
        delete(tr[i]);
    }
}

void Cmd_allocate(char* tr[]){
    if (tr[0]== NULL) printBlockList(tr[0], BlockList);
    else if(!strcmp(tr[0],"-malloc")){
        if (tr[1]== NULL) {
            printBlockList(tr[0], BlockList);
            return;
        }
        void* p;
        if((p = malloc(atoi(tr[1])))==NULL) {
            perror("Error:");
            return;
        }
        else if (atoi(tr[1])==0) printf("0-byte blocks are not assignable\n");
        else{
            printf("%d bytes assigned in %p\n",atoi(tr[1]),p);
            InsertBlock(BlockList, p, atoi(tr[1]), 0, 0, NULL);
        }
    }
    else if(!strcmp(tr[0],"-shared")){
        struct block* myblock = getblock(atoi(tr[1]),NULL,0);
        if(myblock!=NULL){
            size_t tam=(size_t) myblock->size;
            struct block* newblock = shmat(shmget(atoi(tr[1]),tam,0777),NULL,0);
            printf("Shared memory of key %d in %p\n",atoi(tr[1]),newblock);
            InsertBlock(BlockList, newblock, tam, atoi(tr[1]), 0, NULL);
        }
        else printf("No such shared key\n");
    }
    else if(!strcmp(tr[0],"-createshared")){
        if (tr[1]== NULL || tr[2]==NULL) {
            printBlockList(tr[0], BlockList);
            return;
        }
        else do_AllocateCreateshared (&tr[1]);
    }
    else if(!strcmp(tr[0],"-mmap")){
        if (tr[1]== NULL) {
            printBlockList(tr[0], BlockList);
            return;
        }
        do_AllocateMmap(&tr[1]);
    }
    else printf("use: allocate[-malloc|-createshared|-shared|-mmap]......\n");
}
void Cmd_deallocate (char* tr[]) {
    if (tr[0]== NULL) {
        printBlockList(tr[0], BlockList);
        return;
    }
    else if (!strcmp(tr[0], "-malloc")) {
        if (tr[1]== NULL) {
            printBlockList(tr[0], BlockList);
            return;
        }
        struct block* myblock = getblock(0,NULL,atoi(tr[1]));
        if (myblock == NULL) {
            printf("Invalid memory block\n");
        }
        else {
            dealmalloc(myblock->memaddress);
        }
    }
    else if(!strcmp(tr[0],"-shared")){
        if (tr[1] == NULL) {
            printBlockList(tr[0], BlockList);
            return;
        }
        struct block* myblock = getblock(atoi(tr[1]),NULL,0);
        if (myblock == NULL)
            printf("Invalid memory block\n");
        else {
            dealshared(myblock->memaddress);
        }
    }
    else if(!strcmp(tr[0],"-delkey")){
        struct block* myblock = getblock(atoi(tr[1]),NULL,0);
        if (myblock == NULL) {
            printf("Invalid key\n");
            return;
        }
        do_DeallocateDelkey (&tr[1]);
    }
    else if(!strcmp(tr[0],"-mmap")){
        struct block* myblock = getblock(0,tr[1],0);
        if (myblock == NULL) {
            printf("Invalid file\n");
        }
        else {
            dealmap(myblock->memaddress);
        }
    }
    else {
        if(blockfromaddress(cadtop(tr[0]))!=NULL){
            struct block* b = blockfromaddress(cadtop(tr[0]));
            if(b->type == mallocs) {
                dealmalloc(b->memaddress);
            }
            else if (b->type == shared){
                dealshared(b->memaddress);
            }
            else if (b->type ==  mapped) dealmap(b->memaddress);
        }
        else printf("memory address %s not allocated\n",tr[0]);
    }
}
void Cmd_io(char* tr[]) {
    if (tr[0] == NULL){
        printf("use: i-o[read|write]......\n");
        return;
    }

    if (strcmp(tr[0],"read")==0){
        do_I_O_read(tr);
    }
    if (strcmp(tr[0],"write")==0){
        do_I_O_write(tr);
    }
}
void Cmd_Memory(char* tr[]){
    if(tr[0]==NULL || !strcmp(tr[0],"-all")) {
        printvars();
        printfuncs();
        printBlockList(NULL, BlockList);
    }
    else if(!strcmp(tr[0],"-funcs")){
        printfuncs();
    }
    else if(!strcmp(tr[0],"-blocks")){
        printBlockList(NULL, BlockList);
    }
    else if(!strcmp(tr[0],"-vars")){
        printvars();
    }
    else if(!strcmp(tr[0],"-pmap")){
        Do_pmap();
    }
    else{
        printf("Invalid option\n");
    }
}
void Cmd_Recursive(char *tr[]){
    if(tr[0]==NULL){
        printf("Error: argument needed\n");
        return;
    }
    Recursiva(atoi(tr[0]));
}
void Cmd_MemFill(char *tr[]){
    size_t cont=0;
    unsigned char byte = ' ';
    if(tr[0] == NULL) return;
    if(tr[1] == NULL) {
        cont=(size_t) 128;
    }
    else if (isNumber(tr[1]) == 0)
        sscanf(tr[1],"%zu",&cont);
    if(tr[2] == NULL){
        tr[2] = (char *) 'A';
        byte ='A';

    }
    else sscanf(tr[2],"%c",&byte);

    printf("Filling %ld bytes of memory with the byte %c(%2x) in beggining at address %s\n",cont,byte,byte,tr[0]);
    LlenarMemoria(cadtop(tr[0]),cont, byte);
}
void Cmd_Memdump (char *tr[]){
    size_t cont = TAMANO;
    void *p;
    if (tr[0]==NULL)
        return;
    if (tr[1]!=NULL){
        cont = (size_t) strtoull (tr[1], NULL,10);
    }
    else {cont =25;}
    p=cadtop(tr[0]);
    printf("Dumping %zu bytes from address %p\n",cont, p);
    ShowMemory(p,cont);
}

void Cmd_deljobs(char* tr[]){
    if(tr[0] == NULL){
        ClearPL(processlist);
        return;
    }
    if(!strcmp(tr[0],"-term")){
        deleteProcesses(FINISHED);
        return;
    }
    if(!strcmp(tr[1],"-sig")){
        deleteProcesses(SIGNALED);
        return;
    }
}
void Cmd_listjobs(char*tr[]){
    if(tr[0]!=NULL){
        printf("FORMAT: [pid] [user] [priority] [time] [state] [(return/signal)]  [command] \n");
        return;
    }
    printJobs();
}
void Cmd_fork (char *tr[])
{
    pid_t pid;
    if ((pid=fork())==0){
        ClearPL(processlist);
        printf ("Exectuing process %d\n", getpid());
    }
    else if (pid!=-1)
        waitpid (pid,NULL,0);
}
void Cmd_priority(char* tr[]){
    if(tr[0]==NULL) {
        printf("Priority of process %d: %d\n",getpid(),getpriority(PRIO_PROCESS,getpid()));
    }
    else if(tr[1]==NULL ){
        printf("Priority of process %s: %d\n",tr[0],getpriority(PRIO_PROCESS,atoi(tr[0])));
    }
    else if ((atoi(tr[1])>=-20 && atoi(tr[1])<=19)) {
        setpriority(PRIO_PROCESS,atoi(tr[0]),  atoi(tr[1]));
        updatePrio(atoi(tr[0]),  atoi(tr[1]));
    }
}
void Cmd_job(char *tr[]) {
    bool fg = false;
    if (tr[0] != NULL && !strcmp(tr[0], "-fg")) fg = true;
    if (tr[(int) fg] == NULL) {
        printJobs();
        return;
    }
    for (int i = 0; processlist[i] != NULL; i++) {
        int pid = processlist[i]->pid;
        if (pid == atoi(tr[(int)fg])) {
            if (fg) {
                deleteSingleProcess(pid);
                waitpid(pid, NULL, 0);
            }
            else printJobinfo(processlist[i]);
        }
    }
}
void Cmd_execute (char* tr[]){
    char *arguments[MAX];
    char * envvar[MAX];
    getArguments(tr, arguments, envvar);
    if (envvar[0]==NULL) {
        if(OurExecvpe(arguments[0], arguments, environ)!=0)
            perror("Impossible to execute");
    }
    else  {
        if(OurExecvpe(arguments[0], arguments, envvar)!=0)
            perror("Impossible to execute");
    }
}
void Cmd_showvar (char* tr[]){
    if (tr[0]==NULL){
        MostrarEntorno (environ, "main arg3");
        return;
    }
    char * getEnv = getenv(tr[0]);
    int var = BuscarVariable(tr[0], environ);
    if (var == -1)
        printf("Error: Couldn't find variable %s\n", tr[0]);
    else{
        printf("With arg3 main %s (%p)<-(%p)\n", envcpy[var], envcpy[var], &envcpy[var]);
        printf("With environ %s (%p)<-(%p)\n", environ[var], environ[var], &environ[var]);
        printf("With getenv %s (%p)<-(%p)\n", getEnv, getEnv, &getEnv);
    }
}
void Cmd_showenv (char* tr[]){
    if (tr[0]==NULL){
        MostrarEntorno (environ, "main arg3");
    }
    else if (!strcmp(tr[0],"-addr")){
        printf ("environ:                %p (stored in %p)\n", environ, &environ);
        printf ("3rd argument of main:   %p (stored in %p)\n", envcpy, &envcpy);
    }
    else if (!strcmp(tr[0],"-environ"))
        MostrarEntorno (environ, tr[0]);

}
void Cmd_Changevar(char* tr[]){
    int ty=0;
    if(tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        printf("Use: %s [-a|-e|-p] var valor\n", tr[0]);
        return;
    }
    if(!strcmp(tr[0],"-e"))
        ty = ChangeVariable (tr[1], tr[2], environ);
    else if(!strcmp(tr[0],"-a"))
        ty = ChangeVariable (tr[1], tr[2], envcpy);
    else if(!strcmp(tr[0],"-p")) {
        char buf[MAX];
        sprintf(buf,"%s=%s",tr[1],tr[2]);
        ty = putenv(buf);
    }
    else{
        printf("Use: %s [-a|-e|-p] var valor\n",tr[-1]);
    }
    if(ty ==-1) perror("Impossible to change variable");
}
//this is not a command to type in the shell but was also demanded by exercise specification
void Cmd_extra (char * tr[]){
    char* arguments[MAX];
    char* envvar[MAX];
    int i;
    int prio;
    pid_t pidson;

    if((pidson=fork())==0){
    	getArguments(tr, arguments, envvar);
        if (envvar[0]==NULL) {
            if(OurExecvpe(arguments[0], arguments, environ)!=0){
                perror("Impossible to execute");
                exit(EXIT_FAILURE);
                return;
            }
        }

        else  {
            if(OurExecvpe(arguments[0], arguments, envvar) != 0){
                perror("Impossible to execute");
                exit(EXIT_FAILURE);
                return;
            }
        }
    }
    if(checkBack(tr)){
        for (i=0;tr[i] != NULL;i++) {
            if (tr[i][0] == '@') {
                for(int b = 0; b < 3 ; b++){
                    tr[i][b] = tr[i][b + 1];
                }
                setpriority(PRIO_PROCESS, pidson, atoi(tr[i]));
                prio= atoi(tr[i]);
            }
            else prio=getpriority(PRIO_PROCESS, pidson);
        }
        joinString(tr);
        insertProcess(processlist, pidson,tr[0], prio);
        //The priority works the exactly same way as in the provided shell
    }

    else{
        waitpid (pidson, NULL, 0);
    }
}
//---------------------------------------------------------------------
int TrocearCadena(char *cadena, char *trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL){
        return 0;}
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void ProcesarEntrada (char* tr[]){
    int i ;
    if(tr[0]==NULL)
        return;
    for( i = 0; c[i].name != NULL; i++){
        if(!strcmp(c[i].name, tr[0])){
            (*c[i].func)(tr+1);
            return;
        }
    }
    Cmd_extra(tr);
}

int main(int argc, char *argv[],char *envp[]){
    char entrada [MAX];
    char *trozos[MAX];
    envcpy = envp;

    while(1){
        printf("($_$)→ ");
        fgets(entrada,MAX,stdin);
        InsertCommand(entrada);
        if(TrocearCadena(entrada,trozos)==0 ){continue;}
        ProcesarEntrada(trozos);
    }
}
