/******************
 * Polynomial fit *
 * Jason Artz     *
 ******************/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define MAX_DEGREE 12

/* global variables */
    double *x,*y;
    double matrix[MAX_DEGREE+1][MAX_DEGREE+1];
    int datalen;
    long double rh[MAX_DEGREE+1]={0}; 
    double co[MAX_DEGREE*2+1]={0}; 
    double coef[MAX_DEGREE+1];
    char stamp[80];
    char channel[80];

int writedata(char *filename, char degree) {
/****************
 * Write output *
 ****************/
    int i;
    FILE *fp;
    char string[500],s[30];

    sprintf(string,"polyn(%s,%i,",channel,degree);

    if((fp=fopen(filename,"w"))==NULL) {
        printf("Cannot open file.\n");
        return(1);
    }
    
    fprintf(fp,"End data timestamp:\n%s\n\n",stamp);
    fprintf(fp,"Coefficients:\n");
    
    for(i=0;i<=degree;i++) {
        fprintf(fp,"%e\n",coef[i]);
        sprintf(s,"%e",coef[i]);
        strcat(string,s);
        if(i<degree)
            strcat(string,",");
    }

    strcat(string,")");
    
    fprintf(fp,"\nIADS derived parameter:\n%s\n",string);

    fclose(fp);
    return(0);
}

int readdata(char *filename) {
/********************
 *  Read IADS csv   *
 *  Time,ch1,ch2\n  *
 ********************/

    int i,linecount=0;
    FILE *fp;
    char a[280],b[80],c[80];

        if((fp=fopen(filename,"r")) == NULL) {
        printf("Cannot open file.\n");
        return(1);
    }

    while((fscanf(fp,"%s",a)) != EOF) {
        linecount++;
    }

    x=(double *)malloc((linecount)*sizeof(double));
    y=(double *)malloc((linecount)*sizeof(double));
    datalen=linecount-2;

    rewind(fp);

    for(i=0;i<linecount-1;i++) {
        fscanf(fp,"%[^,],%[^,],%s",a,b,c);
        if(i>0) {
            x[i-1]=atof(b);
            y[i-1]=atof(c);
        } else strcpy(channel,b);
    }
    strcpy(stamp,a);
    fclose(fp);
    return(0);
}



void buildmatrix(char degree) {
/******************
 * Make system of *
 * linear eqns    *
 ******************/
    int i,j;
    i=0;
    /* build least squares right hand matrix */
    for (i=0;i<=degree;i++) { 
        for (j=0;j<datalen;j++) {
            if(x[j] == 0.0) rh[i]=rh[i];
            else
                rh[i]=rh[i] + pow(x[j],i)*y[j];
            /* domain error in pow?!?!? */           
        }
    }

    /* build least squares coefficients */
    for (i=0;i<=degree*2;i++) {
        for (j=0;j<datalen;j++) {
            if(x[j] == 0.0) co[i]=co[i];
            else
                co[i]=co[i]+pow(x[j],i);
            /* domain error in pow?!?!? */
        }
    }

    /* build matrix of coefficients */
    for (i=0;i<=degree;i++) {
        for (j=0;j<=degree;j++) {
            matrix[i][j] = co[i+j];
        }
    }

    /* make extended matrix */
    for (i=0;i<=degree;i++) {
        matrix[i][degree+1]=rh[i];
    }
}

void solvesys(char degree) {
/****************
 * Solve system *
 * of lin eqns  *
 ****************/
    int col=0;
    int col2,row2,row=0;
    double t,s;

    for (row=0;row<=degree;row++) {
        col2=row;
        /* turn pivot to 1 */
        t=matrix[row][row];
        for (col=0;col<=degree+1;col++) {
            matrix[row][col]=matrix[row][col]/t;
        }

        /* multiply other row by factor and subtract */
        for (row2=0;row2<=degree;row2++) {           
            if (row2 != row) {
                s=matrix[row2][row];
                for (col=0;col<=degree+1;col++) {
                    matrix[row2][col]=matrix[row2][col] - matrix[row][col]*s;                   
                }
            }
        }        
    }        
}

int main(int argc, char **argv) {

    char degree;
    int i,j;
    
    printf("\nLeast squares polynomial curve fitter\n");
    printf("By Jason Artz\nCompiled on %s\n\n",__DATE__);

    if (argc != 4) {
        printf("Usage: fitpoly <degree (1-%i)> <inputdata.txt> <outputdata.txt>\n",MAX_DEGREE);
        exit(EXIT_FAILURE);
    }

    degree=atoi(argv[1]);
    
    if (degree > MAX_DEGREE || degree < 1) {
        printf("\nDegree must be between 1 and %i\n\n",MAX_DEGREE);
        exit(EXIT_FAILURE);
    }

    if(readdata(argv[2])==1) {
        printf("\nCould not read %s\n",argv[2]);
        exit(EXIT_FAILURE);
    }

        
    buildmatrix(degree);
    
    printf("Least squares extended matrix: \n\n");
    for (i=0;i<=degree;i++) {
        for (j=0;j<=degree+1;j++) {
            printf("%.2e ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    solvesys(degree);
    
    /* show solved matrix */
    printf("Solved matrix: \n\n");
    for (i=0;i<=degree;i++) {
        for (j=0;j<=degree+1;j++) {
            printf("%.2f ",matrix[i][j]);
        }
        coef[i]=matrix[i][j-1];
        printf("\n");
    }
    /* show only coefficients */
    printf("\n\nPolynomial coefficients: \n\n");
    for (i=0;i<=degree;i++) {
        printf("Coefficient %i = %e\n",i,coef[i]);
    }

    printf("\nProcessed %i data points.\n",datalen);

/* write data to file */

    if(writedata(argv[3],degree)==1) {
        printf("Could not write to %s.\n",argv[3]);
        exit(EXIT_FAILURE);
    }

    /* done */


/* debugging 
    for (i=0;i<datalen;i++) {
        printf("%f     %f\n",x[i],y[i]);
    }
*/
    free(x);
    free(y);
    exit(EXIT_SUCCESS);

}
