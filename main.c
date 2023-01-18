#include "parser.h"

int next=0;
pid_t p;

void handle_sigint(int sig)
{
    printf("\nCtrl-C catched. But currently there is no foreground process running.\nmyshell> ");
}
void other(int sig)
{
    printf("\nmyshell> ");
}

void back(struct cmd *cmd){
	struct execcmd *ecmd = (struct execcmd*)cmd;
            
				
				p=fork();
				
    
    			if (p == -1) {
  	 				fprintf(stderr, "fork failed\n");
  	 			}
    			if (p == 0) {
  
  	   			execvp(ecmd->argv[0], ecmd->argv);
  	   			exit(1);
    			}
    			
}

void printcmd(struct cmd *cmd)
{
	 signal(SIGINT, handle_sigint);
	 int f[2];
	 int p[2];
	 pid_t pid1;
	 pid_t pid2;
	 pid_t pid3;
    struct backcmd *bcmd = NULL;
    struct execcmd *ecmd = NULL;
    struct listcmd *lcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct redircmd *rcmd = NULL;

    int i = 0;
    
    if(cmd == NULL)
    {
        PANIC("NULL addr!");
        return;
    }
    

    switch(cmd->type){
        case EXEC:
        		
            ecmd = (struct execcmd*)cmd;
            if(ecmd->argv[0] == 0)
            {
                goto printcmd_exit;
            }
				
				pid1=fork();
    
    			if (pid1 == -1) {
  	 				fprintf(stderr, "fork failed\n");
  	 			}
    			if (pid1 == 0) {
    				if(next!=0){
    					close(next);
    				}
    				
  	   			execvp(ecmd->argv[0], ecmd->argv);
  	   			exit(1);
    			}
    			if (pid1 > 0) {
    				wait(NULL);
      
    			}

            break;

        case REDIR:
        		
            rcmd = (struct redircmd*)cmd;
            ecmd = (struct execcmd*)rcmd->cmd;
            
            
    				if (0 == rcmd->fd_to_close)
            	{
            		int saved = dup(0);
               	char word[30];
    					int counter;
               	int fdr = open(rcmd->file, O_RDONLY);
               	dup2(fdr, 0);
               	
               	for(int x=0; x<MAXARGS; x++){
  	   					if(ecmd->argv[x]==0){
  	   						counter =x;
  	   						x=MAXARGS;
  	   					}
  	   				}
    				
    					while(scanf("%s", word)!=EOF){
    						
            			ecmd->argv[counter]=strdup(word);
							counter++;
  	   					
  	   				}
               	
               	printcmd((struct cmd*)ecmd);
               	dup2(saved,0);
            	}
            	else if (1 == rcmd->fd_to_close)
            	{
               	int saved = dup(1);
               	int fdw = open(rcmd->file, O_RDWR | O_TRUNC | O_CREAT, 0777);
               	dup2(fdw, 1);
               	printcmd(rcmd->cmd);
  	   				dup2(saved,1);
               	
            	}
            	else
            	{
               	PANIC("");
            	}
  	   			
    		
            break;

        case LIST:
            lcmd = (struct listcmd*)cmd;
				
            printcmd(lcmd->left);
            MSG("\n\n");
            printcmd(lcmd->right);
            
            break;

        case PIPE:
            pcmd = (struct pipecmd*)cmd;
            int f[2];
            if(pipe(f)==-1){
    				fprintf(stderr, "pipe failed\n");
    			};
            
				int saved = dup(1);
				int saved2 = dup(0);
				
				dup2(f[1],1);
				dup2(next,0);
            printcmd(pcmd->left);
            dup2(saved, 1);
            dup2(saved2, 0);
            
            
            saved = dup(0);
            saved2 = dup(1);
            
            next = f[0];
            
            if(pcmd->right->type==EXEC){close(f[1]);}
            
            dup2(next,0);         
            printcmd(pcmd->right);
            dup2(saved,0);
	            
	         close(f[0]);
	         close(f[1]);
	         

            break;

        case BACK:
            bcmd = (struct backcmd*)cmd;
				
            back(bcmd->cmd);

            break;


        default:
            PANIC("");
    
    }
    
    printcmd_exit:

    return;
}


int main(void)
{
    static char buf[1024];
    int fd;
    
    signal(SIGINT, handle_sigint);

    setbuf(stdout, NULL);

    // Read and run input commands.
    while(getcmd(buf, sizeof(buf)) >= 0)
    {
        struct cmd * command;
        command = parsecmd(buf);
        printcmd(command); // TODO: run the parsed command instead of printing it
        if(-1==waitpid((p),NULL, WNOHANG)){
        		kill(p,0);
        }
    }
    PANIC("getcmd error!\n");
    return 0;
}
