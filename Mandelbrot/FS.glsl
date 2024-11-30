#version 330
in vec4 frag_color;

vec4 mandl(float x , float y)
{
float c_real = x;
float c_img = y;

float z_real = x;
float z_img = y;
int i;
for(i = 0 ; i < 50 ; i++)
{
float temp = z_real;
 z_real =( z_real*z_real - z_img *z_img)+c_real;
 z_img = 2*temp*z_img+c_img;
 if(z_real*z_real+z_img*z_img > 4) break;
}
return vec4(i/50,i/50,0,1);

}

void main()
{
	gl_FragColor = mandl(frag_color.x*2,frag_color.y*2);
}