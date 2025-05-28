/*************************************************************************
  > 完整实现了2048游戏的功能，并且添加了开机动画
 ************************************************************************/

#include "2048.h"
#include "bmp.h"

int* p_lcd = NULL; // 屏幕映射内存的起始地址

int g_lcd_width; //lcd屏幕的宽
int g_lcd_high;  //lcd屏幕的高
int g_lcd_bpp;   //每个像素点所占比特位
//游戏结束标志
bool game_over = false;


//将所有的图片名保存到一个数组中
const char * bmpfiles[] =	
{
	"game/res/2.bmp", 
	"game/res/4.bmp",
	"game/res/8.bmp",
	"game/res/16.bmp",
	"game/res/32.bmp",
	"game/res/64.bmp",
	"game/res/128.bmp",
	"game/res/256.bmp",
	"game/res/512.bmp",
	"game/res/1024.bmp",
	"game/res/2048.bmp",
	"game/res/4096.bmp",
	"game/res/8192.bmp",
	"game/res/16384.bmp",
	"game/res/32768.bmp",
	"game/res/65536.bmp",
};

//棋盘矩阵
int matrix[BOARDSIZE][BOARDSIZE] =
{
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0	
};

/*
get_bmpfiles_index:根据你要显示的数字(2,4,8,16,...)
	返回你对应的文件名的下标
返回值:
	返回 x对应的文件名在数组bmpfiles的下标
*/
int get_bmpfiles_index(int x)
{
	int exp;
	for(exp=0; x!=0; x >>= 1, exp++);

	return exp-2;
}

/*
	get_zero_num:求棋盘矩阵里面有多少个0
	返回值:
		返回棋盘矩阵中0的个数
*/
int get_zero_num(void)
{
	int z = 0;//棋盘矩阵中元素为0的个数
	int i, j;

	//BOARDSIZE = 4,整个棋盘大小为BOARDSIZE*BOARDSIZE
	for (i = 0; i < BOARDSIZE; i++)
	{
		for (j = 0; j < BOARDSIZE; j++)
		{
			if (matrix[i][j] == 0)
			{
				z++;
			}
		}
	}

	return z;
}

/*
	set_matrix:给棋盘矩阵第z个0的位置，填充一个
		值s
*/
void set_matrix(int z,  int s)
{
	int i, j;
	int k = 0 ;//0的个数

	for (i = 0; i < BOARDSIZE ;i++)
	{
		for (j = 0; j < BOARDSIZE; j++)
		{
			if (matrix[i][j] == 0)
			{
				k++;
				if (k == z)
				{
					matrix[i][j] = s;
					return ;
				}
			}
			
		}
	}
}

/*
 * x: x轴坐标
 * y: y轴坐标
 * color: 要填充的颜色
*/
void lcd_draw_point(int x, int y, int color)
{
    if (x >= 0 && x < g_lcd_width && y >=0 && y < g_lcd_high)
    {
        *(p_lcd + g_lcd_width*y + x) = color;
    }
}

/*
 * @：画一个矩形
 * x0:代表矩形的起始点X轴坐标
 * y0:代表矩形的起始点y轴坐标
 * w: 屏幕的宽度
 * h: 屏幕的高度
 * color: 矩形填充的颜色
*/
void lcd_draw_rect(int x0, int y0, int w, int h, int color)
{
    if (x0 < 0 || y0 < 0 || w < 0 || h <0)
    {
        printf("draw_rect: param error!\n");
        return;
    }

    if ((x0+w > g_lcd_width) || (y0+h > g_lcd_high))
    {
        printf("draw_rect: param error!\n");
        return;
    }

    // 画一个矩形
    for (int y = y0; y < y0+h; y++) // 行 --> 屏幕的高
    {
        for (int x = x0; x < x0+w; x++) // 列 --> 屏幕的宽
        {
            // 画一个像素点
            lcd_draw_point(x, y, color);
        }
    }
}

/*
 * 加载一张bmp图片
 * bmpfile: 待加载图片的文件名
 * x0: X轴坐标
 * y0：Y轴坐标
*/
void draw_bmp(const char* bmpfile, int x0, int y0)
{
    // 打开bmp文件
    int fd = open(bmpfile, O_RDONLY);
    if (fd == -1)
    {
        perror("open bmpfile error");
        return;
    }

    // 获取文件属性,并偏移bmp图片的文件格式的54个字节
    struct header head;
    struct info in;
    read(fd, &head, sizeof(head));
    read(fd, &in, sizeof(in));

    // 动态开辟内存，用于保存取出出来的像素点
    char* bmpdata = calloc(1, head.size-head.offbytes);

    // 从bmp图片中读取所有的像素点信息
    read(fd, bmpdata, in.width*in.height*in.bit_count/8);

    // 像素点信息已经读取完成，文件就可以关闭了
    close(fd);

    int i = 0;
    for (int y = 0; y < in.height; y++)
    {
        unsigned char r, g, b;
        int color;
        for(int x = 0; x < in.width; x++)
        {
            // bmp图片中像素点读取的顺序是 bgr， lcd屏幕写入时需要按照rgb顺序来写
            b = bmpdata[i++];
            g = bmpdata[i++];
            r = bmpdata[i++];

            // bmp图片一个像素点占3个字节，即 RGB
            // 屏幕一个像素点占4个字节，即 ARGB
            color = (r << 16) | (g << 8) | b;

            lcd_draw_point(x0+x, y0+(in.height-1-y), color);
        }
    }
	free(bmpdata);
}

/*
 * 画2048的棋盘矩阵 
*/
void draw_matrix()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int x0 = 185, y0 = 25; // 当前矩阵中，第一个矩形的起始点坐标
            if (matrix[i][j] == 0)
			{
                //如果此处元素的值为0，那么就显示
				lcd_draw_rect(x0+j*110,  y0+i*110, 100, 100, 0xb4eeb4);
			}
			else
			{
                // 画对应的图片
				int f_index = get_bmpfiles_index(matrix[i][j]);
				draw_bmp(bmpfiles[f_index], x0+j*110, y0+i*110);
			}
        }
    }
}

/*
	init_matrix:初始化棋盘矩阵
			在任意x个位置，填充x个数字(2,4,8)
*/
void init_matrix(void)
{
	//规则x >= 1,x <= 3
	int x = (random() % 3) + 1;

	int i;

	/*
		step1:随机产生x个数字，并填充到棋盘矩阵中去
	*/
	for(i = 0; i < x; i++)
	{
		int pos = (random() % get_zero_num()) + 1;

		int s[] = {2, 4, 8, 2};
		int s_i = (random() % 3);

		set_matrix(pos, s[s_i]);
	}

	/*
		step 2: 绘制棋盘矩阵
	*/
	draw_matrix();
}

/*
	rand1_matrix:移动之后随机产生一个数字填充到
	任意一个0的位置上
*/
void rand_matrix()
{
	int pos = (random() % get_zero_num()) + 1;

	int s[] = {2, 4, 8, 2};
	int s_i = (random() % 4);

	set_matrix(pos, s[s_i]);
	draw_matrix();
}

/*
	判断是否还能移动
*/
bool is_game_over(void)
{
	int i, j;
	if(get_zero_num() != 0)
	{
		return false;
	}

	for(i = 0; i < BOARDSIZE; i++)
	{
		for(j = 0; j < BOARDSIZE ; j++)
		{
			if (j != BOARDSIZE -1)
			{
				if (matrix[i][j] == matrix[i][j+1])
				{
					return false;
				}
			}

			if (i != BOARDSIZE - 1)
			{
				if (matrix[i][j] == matrix[i+1][j])
				{
					return false;
				}
			}
		}
	}
	
	return true;
}

/*
 * 获取手指在触摸屏上面的滑动方向
 * 返回值：
 *      MOVE_LEFT
 *      MOVE_RIGHT
 *      MOVE_UP
 *      MOVE_DOWN
*/
int get_figer_direction()
{
    int fd = open("/dev/input/event0", O_RDONLY);
    if (fd == -1)
    {
        perror("open event0 fail");
        return -1;
    }

    struct input_event ev; // 用于获取触摸屏属性信息
    int x1 = -1; // 在滑动过程中第一个点的X轴坐标
    int x2 = -1;      // 在滑动过程中最后一个点的X轴坐标
    int y1 = -1; // 在滑动过程中第一个点的Y轴坐标
    int y2 = -1;      // 在滑动过程中最后一个点的Y轴坐标
    while(1)
    {
        read(fd, &ev, sizeof(ev));
        if (ev.type == EV_ABS && ev.code == ABS_X) // X轴坐标
        {
            if (x1 == -1) // x1未被赋值，代表其为第一个点坐标
            {
                x1 = ev.value / 1024.0 * 800;
            }
            
            x2 = ev.value / 1024.0 * 800;
        }

        if (ev.type == EV_ABS && ev.code == ABS_Y) // Y轴坐标
        {
            if (y1 == -1) // y1未被赋值，代表其为第一个点坐标
            {
                y1 = ev.value / 600.0 * 480;
            }
            
            y2 = ev.value / 600.0 * 480;
        }

        // 手指弹起，再计算滑动方向
        if (ev.type == EV_KEY 
                && ev.code == BTN_TOUCH 
                && ev.value == 0)
        {
            int x_cz; // x轴的位移
            int y_cz; // y轴的位移
            int abs_x;
            int abs_y;

            x_cz = x2 - x1;
            y_cz = y2 - y1;

            abs_x = abs(x_cz);
            abs_y = abs(y_cz);

            if ((x_cz > 30) && (abs_x > 2*abs_y))
            {
                close(fd);
                return MOVE_RIGHT;
            }
            else if ((x_cz < -30) && (abs_x > 2*abs_y))
            {
                close(fd);
                return MOVE_LEFT;
            }
            else if ((y_cz > 30) && (abs_y > 2*abs_x))
            {
                close(fd);
                return MOVE_DOWN;
            }
            else if ((y_cz < -30) && (abs_y > 2*abs_x))
            {
                close(fd);
                return MOVE_UP;
            }
            else
            {
                x1 = -1;
                y1 = -1;
                continue;
            }
        }
    }

    close(fd);
}

// 手指左滑
void fin_left()
{
    int i, j; // i为矩阵行下标，j为矩阵列下标
    int value;
    int save_zero;
    for (i = 0; i < BOARDSIZE; i++)
    {
        value = 0;
        save_zero = 0;
        for (j = 0; j < BOARDSIZE; j++)
        {
            if (matrix[i][j] == 0)
            {
                continue;
            }
            
            if (value == 0)
            {
                value = matrix[i][j];
            }
            else
            {
                if (value == matrix[i][j])
                {
                    matrix[i][save_zero++] = 2 * value;
                    value = 0;
                }
                else
                {
                    matrix[i][save_zero++] = value;
                    value = matrix[i][j];
                }
            }
            matrix[i][j] = 0;
        }
        
        if (value != 0)
        {
            matrix[i][save_zero] = value;
        }
    }
}

// 手指右滑
void fin_right()
{
	int i, j;//i为矩阵行下标，j为矩阵列下标
	int value;
	int save_zero;
	for (i = 0; i < BOARDSIZE; i++)
	{
		value = 0;
		save_zero = BOARDSIZE -1;
		for (j = BOARDSIZE - 1; j >= 0 ; j--)
		{
			if(matrix[i][j] == 0)
			{
				continue;
			}
            
			if(value == 0)
			{
				value = matrix[i][j];
			}
			else
			{
				if(value == matrix[i][j])
				{
					matrix[i][save_zero--] = 2 * value;
					value = 0;
				}
				else
				{
					matrix[i][save_zero--] = value;
					value = matrix[i][j];
				}
			}
			matrix[i][j] = 0;
		}
        
		if(value != 0)
		{
			matrix[i][save_zero] = value;
		}
	}
}

// 手指上滑
void fin_up()
{
	int i, j; // i为矩阵行下标，j为矩阵列下标
    int value;
    int save_zero;

    for (j = 0; j < BOARDSIZE; j++) {
        value = 0;
        save_zero = 0; // 用于保存非零元素的目标行位置，初始指向最上方
        for (i = 0; i < BOARDSIZE; i++) { // 从上到下遍历行
            if (matrix[i][j] == 0) {
                continue; // 如果当前元素为0，跳过
            }

            if (value == 0) {
                value = matrix[i][j]; // 如果value为空，将当前元素赋值给value
            } else {
                if (value == matrix[i][j]) { // 如果当前元素与value相等
                    matrix[save_zero++][j] = 2 * value; // 合并两个相同的元素，并将结果存储在save_zero的位置
                    value = 0; // 重置value
                } else {
                    matrix[save_zero++][j] = value; // 如果不相等，将value存储在当前位置，并将save_zero下移一位
                    value = matrix[i][j]; // 更新value为当前元素
                }
            }
            matrix[i][j] = 0; // 将当前元素置为0，表示已处理
        }

        if (value != 0) {
            matrix[save_zero][j] = value; // 如果循环结束后value不为空，将其存储在最后的位置
        }
	}
}

// 手指下滑
void fin_down()
{
	int i, j;//i为矩阵行下标，j为矩阵列下标
	int value;
	int save_zero;
	
	for(j = 0; j < BOARDSIZE; j++)
	{
		value = 0;
		save_zero = BOARDSIZE - 1;
		for(i = BOARDSIZE - 1; i >= 0 ; i--)
		{
			if(matrix[i][j] == 0)
			{
				continue;
			}
            
			if(value == 0)
			{
				value = matrix[i][j];
			}
			else
			{
				if(value == matrix[i][j])
				{
					matrix[save_zero--][j] = 2 * value;
					value = 0;
				}
				else
				{
					matrix[save_zero--][j] = value;
					value = matrix[i][j];
				}
			}
			matrix[i][j] = 0;
		}
        
		if(value != 0)
		{
			matrix[save_zero][j] = value;
		}
	}
}

/*
 * 根据手指滑动来变换棋盘矩阵
*/
void change_matrix()
{
    int dir = get_figer_direction();
    if (dir == MOVE_LEFT)
    {
        fin_left();
    }
    else if (dir == MOVE_RIGHT)
    {
        fin_right();
    }
    else if (dir == MOVE_UP)
    {
        fin_up();
    }
    else if (dir == MOVE_DOWN)
    {
        fin_down();
    }
}

//显示开机动画
void show_gif()
{
	int i;
	char buf[32] = {0};
	for(i=0; i<442; i++)
	{	
		sprintf(buf, "./game/gif_bmp/%d.bmp", i);
		printf("%s\n", buf);
		draw_bmp(buf, 0, 0);
	}
}

void game2048(void) {
    // 打开屏幕所对应的设备文件
    // fb0 代表显示屏的设备文件

	// 动态获取屏幕信息
	
    g_lcd_width = 800;
    g_lcd_high  = 480;
    g_lcd_bpp   = 32;

    // 整个屏幕的大小(单位：byte)
    int screen_size = g_lcd_width * g_lcd_high * g_lcd_bpp/8;

    // 将lcd屏幕的物理内存映射为虚拟内存
    p_lcd = openLcd();

    // 显示开机动画
    show_gif();

    // 清屏
    lcd_draw_rect(0, 0, g_lcd_width, g_lcd_high, 0xffffff);

	srandom(time(NULL)); //设置随机数种子
	init_matrix();  // 初始化棋盘矩阵    

	while (!game_over) //游戏没结束时
	{
		//用来保存原来的矩阵值
		int matrix_v1[BOARDSIZE][BOARDSIZE];
		int i, j, flag = 0;
		for(i = 0; i < BOARDSIZE; ++i)
		{
			for (j = 0; j < BOARDSIZE; ++j)
			{
				matrix_v1[i][j] = matrix[i][j];
			}
		}

		// 变换矩阵
		change_matrix();

        // 矩阵变换后，重新改变数组中的元素值
		for (i = 0; i < BOARDSIZE; ++i)
		{
			for (j = 0; j < BOARDSIZE; ++j)
			{
				if (matrix_v1[i][j] != matrix[i][j])
				{
					flag = 1;
					i = j = BOARDSIZE;
				}
			}
		}
        
        // flag = 1 ： 代表“变换矩阵”后有变化
        // flag = 0 ： 代表“变换矩阵”后没有变化
		if (flag)
		{
            // 空白处产生随机的2、4、8值
			rand_matrix();
            
            // 重新绘制棋盘
			draw_matrix();
		} 
		else 
		{
            // 绘制棋盘
			draw_matrix();
		}
        
        // 判断游戏是否结束
		game_over = is_game_over();
	}

    // 游戏结束后
	lcd_draw_rect(0, 0, g_lcd_width, g_lcd_high, 0xffffff);//清屏
	draw_bmp("game/res/game_over.bmp", 250, 165);

    // 关闭屏幕的设备文件

    // 解除映射
    munmap(p_lcd, screen_size);

    return;
}
