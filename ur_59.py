
import serial

# Change the port and baudrate to match your Arduino's configuration
arduino_port = 'COMx'  # Replace 'COMx' with your Arduino's serial port
baudrate = 9600

try:
    arduino = serial.Serial(arduino_port, baudrate)
    print(f"Connected to Arduino on {arduino_port}")

    while True:
        # Read a line of data from the Arduino
        data = arduino.readline().decode().strip()
        print(data)

except serial.SerialException as e:
    print(f"Failed to connect to Arduino: {e}")
finally:
    if arduino.is_open:
        arduino.close()

import pygame
import random

# Initialize Pygame
pygame.init()

# Set up the screen
screen_width = 800
screen_height = 600
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Shooting Game")

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)

# Fonts
font = pygame.font.SysFont(None, 36)

# Player
player_size = 50
player_pos = [screen_width // 2, screen_height - 2 * player_size]
player_speed = 10
player_color = GREEN
player_lives = 3

# Bullets
bullet_width = 5
bullet_height = 15
bullet_speed = 10
bullets = []

# Enemies
enemy_size = 50
enemy_speed = 5
enemies = []

# Explosions
explosions = []

# Score
score = 0

# Clock
clock = pygame.time.Clock()

# Functions
def draw_text(text, color, x, y):
    text_surface = font.render(text, True, color)
    screen.blit(text_surface, (x, y))

def player_hit():
    global player_lives
    player_lives -= 1
    if player_lives == 0:
        game_over()

def game_over():
    global running
    running = False

def create_explosion(x, y):
    explosion_color = random.choice([YELLOW, RED, WHITE])
    explosion_radius = random.randint(10, 30)
    explosions.append([x, y, explosion_radius, explosion_color])

def draw_explosions():
    for explosion in explosions:
        pygame.draw.circle(screen, explosion[3], (explosion[0], explosion[1]), explosion[2])
        explosion[2] += 2
        if explosion[2] >= 50:
            explosions.remove(explosion)

# Game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                bullets.append([player_pos[0] + player_size // 2 - bullet_width // 2, player_pos[1]])

    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT] and player_pos[0] - player_speed > 0:
        player_pos[0] -= player_speed
    if keys[pygame.K_RIGHT] and player_pos[0] + player_size + player_speed < screen_width:
        player_pos[0] += player_speed

    screen.fill(BLACK)

    # Draw player
    pygame.draw.rect(screen, player_color, (player_pos[0], player_pos[1], player_size, player_size))

    # Draw bullets
    for bullet in bullets:
        pygame.draw.rect(screen, WHITE, (bullet[0], bullet[1], bullet_width, bullet_height))
        bullet[1] -= bullet_speed
        if bullet[1] < 0:
            bullets.remove(bullet)

    # Add enemies
    if len(enemies) < 10:
        if random.randint(0, 100) < 5:
            enemy_pos = [random.randint(0, screen_width - enemy_size), 0]
            enemies.append(enemy_pos)

    # Draw enemies
    for enemy in enemies:
        pygame.draw.rect(screen, RED, (enemy[0], enemy[1], enemy_size, enemy_size))
        enemy[1] += enemy_speed
        if enemy[1] > screen_height:
            enemies.remove(enemy)

    # Check for collisions
    for enemy in enemies:
        for bullet in bullets:
            if (bullet[0] > enemy[0] and bullet[0] < enemy[0] + enemy_size and
                    bullet[1] > enemy[1] and bullet[1] < enemy[1] + enemy_size):
                bullets.remove(bullet)
                enemies.remove(enemy)
                score += 1
                create_explosion(enemy[0] + enemy_size // 2, enemy[1] + enemy_size // 2)

        if (player_pos[0] + player_size > enemy[0] and player_pos[0] < enemy[0] + enemy_size and
                player_pos[1] + player_size > enemy[1] and player_pos[1] < enemy[1] + enemy_size):
            enemies.remove(enemy)
            player_hit()
            create_explosion(player_pos[0] + player_size // 2, player_pos[1] + player_size // 2)

    # Draw explosions
    draw_explosions()

    # Draw score and lives
    draw_text(f"Score: {score}", WHITE, 10, 10)
    draw_text(f"Lives: {player_lives}", WHITE, screen_width - 100, 10)

    pygame.display.update()
    clock.tick(30)

pygame.quit()

