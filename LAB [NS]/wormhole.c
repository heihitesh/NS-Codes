#include<stdio.h>
#include<stdlib.h>
int normal_mat[50][50], for_worm[50][50],p[50][50];
int inf=1000,v;
void compare(int normal_mat[v][v], int for_worm[v][v], int v)
{
  int i,j,flag=0;
  for(i=0;i<v;i++)
  {
    for(j=0;j<v;j++)
    {

      if(normal_mat[i][j]>for_worm[i][j])
    {
      printf("Wormhole detected");

    }
    }

  }
  if(flag==0)
  printf("No wormhole detected");
  for(i=0;i<v;i++)
  {
    for(j=0;j<v;j++)
    {
      printf("%d", normal_mat[i][j]);
    }
    printf("\n");
  }
  for(i=0;i<v;i++)
  {
    for(j=0;j<v;j++)
    {
      printf("%d", for_worm[i][j]);
    }
    printf("\n");
  }
}


void show()
{
  int i,j;

  for(i=0;i<v;i++)

  {

   for(j=0;j<v;j++)

    if(normal_mat[i][j]==1000)

     printf("INF\t");

    else

     printf("%d\t", normal_mat[i][j]);

   printf("\n");

  }

}


void show_worm()
{
  int i,j;

  for(i=0;i<v;i++)

  {

   for(j=0;j<v;j++)

    if(for_worm[i][j]==1000)

     printf("INF\t");

    else

     printf("%d\t", for_worm[i][j]);

   printf("\n");

  }

}

void path(int i, int j)
{
  int k;

  k=p[i][j];

  if(k==-1)

  {

   printf("->");

   return;

  }

  path(i, k);

  printf("%d",k);

  path(k,j);

}

void for_normal(int v)
{
  int i,j,k,x;
  printf("\n matrix without wormhole:");
  srand(time(NULL));
  for(i=0;i<v;i++)
  {
    for(j=0;j<v;j++)
    {
    //  scanf("%d",&normal_mat[i][j]);
    normal_mat[i][j]=(rand() % 2);
    if( normal_mat[i][j]==0)
     normal_mat[i][j]=1000;
      p[i][j]=-1;

    }

  }

  for(k=0;k<v;k++)
  {
    for(i=0;i<v;i++)
    {
      for(j=0;j<v;j++)
      {
        if(i==j)
        normal_mat[i][j]=0;
        else
        {
          x = normal_mat[i][k]+normal_mat[k][j];
          if(normal_mat[i][j]>x)
          {
            normal_mat[i][j] = x;
            p[i][j]=k;
          }
        }
      }
    }
    show();
    printf("\n");
  }

  printf("From\tTo\tPath\t\tTotal Min. Cost\n");

  for(i=0;i<v;i++)

  {

   for(j=0;j<v;j++)

   {

    if(i!=j)

    {

     printf("%d\t", i);

     printf("%d\t", j);

     printf("%d", i);

     path(i,j);

     printf("%d", j);



     printf("\t\t%d", normal_mat[i][j]);

     printf("\n");

    }

   }

 }
 for(i=0;i<v;i++)
 {
   for(j=0;j<v;j++)
   {
     printf("%d", normal_mat[i][j]);
   }
   printf("\n");
 }
}

void for_worm_fun(int v)
{
  int i,j,k,x;
  printf("\nEnter matrix with wormhole:");
  for(i=0;i<v;i++)
  {
    for(j=0;j<v;j++)
    {
      scanf("%d",&for_worm[i][j]);
      p[i][j]=-1;

    }

  }

  for(k=0;k<v;k++)
  {
    for(i=0;i<v;i++)
    {
      for(j=0;j<v;j++)
      {
        if(i==j)
        for_worm[i][j]=0;
        else
        {
          x = for_worm[i][k]+for_worm[k][j];
          if(for_worm[i][j]>x)
          {
            for_worm[i][j] = x;
            p[i][j]=k;
          }
        }
      }
    }
    show_worm();
    printf("\n");
  }

  printf("From\tTo\tPath\t\tTotal Min. Cost\n");

  for(i=0;i<v;i++)

  {

   for(j=0;j<v;j++)

   {

    if(i!=j)

    {

     printf("%d\t", i);

     printf("%d\t", j);



 //    printf("Path from %d to %d is: ",i,j);

     printf("%d", i);

     path(i,j);

     printf("%d", j);



     printf("\t\t%d", for_worm[i][j]);

     printf("\n");

    }

   }

 }

}
int main()
{
  int i,j,k;

  printf("Enter no. of vertices for topology matrix:");
  scanf("%d",&v);

for_normal(v);
for_worm_fun(v);
for(i=0;i<v;i++)
{
  for(j=0;j<v;j++)
  {
    if(normal_mat[i][j]!=for_worm[i][j])
    printf("Wormhole detected\n");
  }
}
//compare(normal_mat, for_worm,v);

return 0;
}
