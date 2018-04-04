def rectangle:house:
	set extrude(6)
	fork front:
		set spritx()
		fork 1*:
			wall()
		fork 435*:
			wall()
		fork 54:
			wall()
		fork top:
			extrude(4)
			color(red)
	fork right:
		nil()
	fork left:
		normal("dd.jpg")

def rectangle:house()
{
	extrude(4)
	:front
	{
		spritx()
		:1*   => wall()
		:435* => wall()
		:54   => wall()
		:top
		{
			extrude(4)
			color(red)
		}
	}
	:right => nul()
	:left  => normal("dd.jpg")

	extrude(4)
	{
	1* => wall()
	435* => wall()
	54 => wall()
	top:


	}
}