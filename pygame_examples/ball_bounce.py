import sys, pygame

pygame.init()  # Initialize all PyGame modules

size = width, height = 800, 640  # Set window size
speed = [2, 2]  # Set vertical and horizontal speed
black = 0, 0, 0  # Set background color

screen = pygame.display.set_mode(size)  # Create window

ball = pygame.image.load("intro_ball.gif")  # Load image
ballrect = ball.get_rect()  # Place image in rectangle

while 1:
    for event in pygame.event.get():  # Check for events
        if event.type == pygame.QUIT: sys.exit()  # Quite and cleanup

    ballrect = ballrect.move(speed)  # Move image rectangle at specified speed
    if ballrect.left < 0 or ballrect.right > width:  # Bounce if exceed window boundaries
        speed[0] = -speed[0]
    if ballrect.top < 0 or ballrect.bottom > height:  # Bounce if exceed window boundaries
        speed[1] = -speed[1]

    screen.fill(black)  # Erase screen
    screen.blit(ball, ballrect)  # Merge ball image to were rectangle is currently located
    pygame.display.flip()  # Show ball at new position