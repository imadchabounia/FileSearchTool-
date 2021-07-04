#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <regex.h>

/*
search [nom_repertoire] [-options] [fichier]
*/

typedef struct dirent dirent;

int cnt_fichiers = 0; //compteur pour le nombre de fichiers trouvés

char* concat(const char* s1, const char* s2){
  //concatener deux const
  char* res = malloc(sizeof(char)*(strlen(s1)+strlen(s2)+1));
  int i;
  for(i=0; i<(int)strlen(s1); i++){
    *(res+i)=s1[i];
  }
  for(i=0; i<(int)strlen(s2); i++){
    *(res+i+(int)strlen(s1))=s2[i];
  }
  *(res+i+(int)strlen(s1))='\0';
  return res;
}

void showDate(time_t t){
  char buffer[26];
  struct tm* tm_info;
  tm_info = localtime(&t);
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
  puts(buffer);
}
void showFileType(mode_t st_mode){
  printf("[+] type de fichier : ");
  if(S_ISREG(st_mode)){
    printf("fichier regulier (-)\n");
  }else if(S_ISFIFO(st_mode)){
    printf("fichier special(FIFO) (p) \n");
  }else if(S_ISCHR(st_mode)){
    printf("Periph mode caractere (c)\n");
  }else if(S_ISBLK(st_mode)){
    printf("Periph mode bloc (b)\n");
  }else if(S_ISDIR(st_mode)){
    printf("repertoire (d)\n");
  }else if(S_ISLNK(st_mode)){
    printf("lien symbolique (l)\n");
  }else if(S_ISSOCK(st_mode)){
    printf("socket (s)\n");
  }else{
    printf("%s\n","");
  }
}
void showPermissions(mode_t st_mode){
  //afficher avec le style linux, (owner, group, other users)
  printf("[+] protection du fichier : ");
  printf( (S_ISDIR(st_mode)) ? "d" : "-");
  printf( (st_mode & S_IRUSR) ? "r" : "-");
  printf( (st_mode & S_IWUSR) ? "w" : "-");
  printf( (st_mode & S_IXUSR) ? "x" : "-");
  printf( (st_mode & S_IRGRP) ? "r" : "-");
  printf( (st_mode & S_IWGRP) ? "w" : "-");
  printf( (st_mode & S_IXGRP) ? "x" : "-");
  printf( (st_mode & S_IROTH) ? "r" : "-");
  printf( (st_mode & S_IWOTH) ? "w" : "-");
  printf( (st_mode & S_IXOTH) ? "x" : "-");
  printf("\n");
}
void afficherFichierInfo(struct stat buf, const char* options){
  int i;
  for(i=0;i<strlen(options);i++){
    switch (options[i]) {
      case 'd':
      //date dernier accés
      printf("[+] date du dernier acces : ");
      showDate(buf.st_atime);
      //date dernière modification
      printf("[+] date de la dernière modification : ");
      showDate(buf.st_mtime);
      break;
      case 'm':
      printf("[+] date de la dernière modification : ");
      showDate(buf.st_mtime);
      break;
      case 't':
      //afficher le type du fichier
      showFileType(buf.st_mode);
      break;
      case 's':
      printf("[+] la taille du fichier en octets : %ld \n", (long )buf.st_size);
      break;
      case 'p':
      //afficher la protection du fichier
      showPermissions(buf.st_mode);
      break;
      case 'a':
      printf("[+] date du dernier acces : ");
      showDate(buf.st_atime);
      printf("[+] date de la dernière modification : ");
      showDate(buf.st_mtime);
      showFileType(buf.st_mode);
      printf("[+] la taille du fichier en octets : %ld \n", (long )buf.st_size);
      showPermissions(buf.st_mode);
      break;
    }
  }
}

struct stat getFileInode(const char* nom_repertoire, const char* nom_fichier){
  char* path = concat(nom_repertoire, nom_fichier);
  struct stat buf;
  if(stat(path, &buf)==-1){
    printf("%s\n", "Impossible de lite (tous ou certains) elements de l'inode(Probleme protection)");
    return buf;
  }
  return buf;
}

int wildcardMatch(const char* s, const char* pattern){
  int n = strlen(s);
  int m = strlen(pattern);
  int dp[n+1][m+1];
  memset(dp, 0, sizeof(dp));
  int i, j;
  dp[0][0] = 1;
  for(j = 1; j <= m; j++){
    if(pattern[j-1]=='*'){
      dp[0][j] = dp[0][j-1];
    }
  }
  for(i = 1; i <= n; i++){
    for(j = 1; j <= m; j++){
      if(pattern[j-1]=='*'){
        if(dp[i-1][j]==1 || dp[i][j-1]==1){
          dp[i][j]=1;
        }
      }else if(pattern[j-1]=='?' || (pattern[j-1]==s[i-1])){
        dp[i][j]=dp[i-1][j-1];
      }
    }
  }
  return dp[n][m];
}

void processFile(const char* nom_repertoire, const char* nom_fichier, const char* options){
  int n = strlen(nom_repertoire);
  int m = strlen(nom_fichier);
  char* tag = malloc(sizeof(char)*(n+m+3));
  strcpy(tag, nom_repertoire);
  if(tag[n-1]!='/'){
    strcat(tag, "/");
  }
  strcat(tag, nom_fichier);
  if(strlen(options)==0){
    if(cnt_fichiers>1){
      printf(", ");
      printf("%s", tag);
    }else{
      printf("%s", tag);
    }
  }else{
    struct stat fileStat = getFileInode(nom_repertoire, nom_fichier);
    printf("[***] \t %s \n", tag);
    printf("--------------------------------------------------------------\n");
    afficherFichierInfo(fileStat, options);
    printf("\n");
  }
  free(tag);
}

char* convertToFitRegex(const char* s){
  int length = strlen(s);
  int i; for(i=0; i<length; i++){
    if(s[i]=='*') length++;
  }
  char* ret = malloc(sizeof(char)*length);
  int j=0;
  for(i=0;i<length;i++){
    if(s[i] == '?'){
      *(ret+j) = '.'; j++;
    }else if(s[i]=='*'){
      *(ret+j) = '.'; j++;
      *(ret+j) = '*'; j++;
    }else{
      *(ret+j) = s[i]; j++;
    }
  }
  return ret;
}

int match(const char* s, const char* pattern){
  regex_t regex;
  int value = regcomp(&regex, pattern, 0);
  if(value!=0){
    printf("probleme de compilation de regex \n");
    return 1;
  }
  value = regexec(&regex, s, 0, NULL, 0);
  if(value==0){
    printf("%s %s\n", s, pattern);
    return 0;
  } // true, pattern found ok
  if(value==REG_NOMATCH) return 1;
  printf("erreur!!!\n");
  return -1;
}
void search_files(const char* nom_repertoire, const char* nom_fichier, const char* options, int niveau){
  if(niveau == -1) return;
  //printf("%s\n", nom_repertoire);
  DIR* rep;
  struct dirent* dEntry;
  rep = opendir(nom_repertoire);
  if(rep == NULL){
    printf("Impossible d'ouvrir le repertoire %s \n", nom_repertoire);
    return;
  }
  while(dEntry = readdir(rep)){
    if(dEntry->d_type == 4){
      const char* nom_repertoire2 = dEntry->d_name;
      if((strcmp(nom_repertoire2, ".")!=0) && (strcmp(nom_repertoire2, "..")!=0)){
        int n = strlen(nom_repertoire);
        int m = strlen(nom_repertoire2);
        char* new_repertoire = malloc(sizeof(char)*(n+m+4));
        strcpy(new_repertoire, nom_repertoire);
        if(new_repertoire[n-1]!='/'){
          strcat(new_repertoire, "/");
        }
        strcat(new_repertoire, nom_repertoire2);
        strcat(new_repertoire, "/");
        search_files(new_repertoire, nom_fichier, options, niveau-1);
        free(new_repertoire);
      }
    }else{
      //leaf node
      //try to match, if it works then afficher
      const char* nom_fichier2 = dEntry->d_name;
      if(wildcardMatch(nom_fichier2, nom_fichier)==1){
        cnt_fichiers++;
        //printf("%s %s\n", nom_fichier2, nom_fichier);
        processFile(nom_repertoire, nom_fichier2, options);
      }
    }
  }
  closedir(rep);
}

void search(const char* nom_repertoire, const char* nom_fichier, const char* options, int depth){
  char* nom_fichier_regex = convertToFitRegex(nom_fichier);
  search_files(nom_repertoire, nom_fichier, options, depth);
  free(nom_fichier_regex);
}

int main(int argc, char const *argv[]) {
  //nom du fichier est obligatoire
  if(argc<2){
    printf("[!] Argument manquant\n");
    printf("%s\n", "[+] Format attendu : ./search [Nom_du_Repertoire] [-options] Nom_de_fichier");
    return 1;
  }
  const char* nom_repertoire;
  int repertoire_exist_arg=1;
  if(argc==2 || (argc>=3 && argv[1][0]=='-')){
    nom_repertoire = "./";
    repertoire_exist_arg=0;
  }else{
    nom_repertoire = argv[1];
  }
  size_t arg_length = argc-1;
  int depth=0;
  char* options;
  if(argc==2){
    options = (char* )malloc(sizeof(char));
  }else{
    options = (char* )malloc(sizeof(char)*arg_length-2);
    int i = 1;
    int k = 0;
    if(repertoire_exist_arg==1) i++;
    for(; i < argc-1; i++){
      if(argv[i][1]<='9' && argv[i][1]>='0'){
        int j = 1;
        while(argv[i][j]<='9' && argv[i][j]>='0'){
          depth *= 10;
          depth += (argv[i][j]-'0');
          j++;
        }
      }else{
        *(options+(k)) = argv[i][1];
      }
    }
  }

  const char* nom_fichier = argv[arg_length]; // may contains wildcard
  printf("%s\n", "");
  search(nom_repertoire, nom_fichier, options, depth);
  printf("%s\n", "");
  free(options);
  return 0;
}
