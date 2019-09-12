#!/usr/bin/env python

import pygame as pg


class PygView(object):
    """Display framerate and playing time"""

    def __init__(self, width=640, height=400, fps=30):
        """Initialize graphics system, window, background, etc."""
        pg.init()
        pg.display.set_caption("Press ESC to quit")
        self.width = width
        self.height = height
        self.screen = pg.display.set_mode((self.width, self.height), pg.DOUBLEBUF)  # Create visible screen; DOUBLEBUF
        # helps reduce flicker
        self.background = pg.Surface(self.screen.get_size()).convert()  # Create empty background screen, then create
        # a new copy of the screen w/ same pixel format (fastest form for blitting)
        self.clock = pg.time.Clock()  # Create object to track time
        self.fps = fps
        self.playtime = 0.0
        self.font = pg.font.SysFont('mono', 20, bold=True)

    def run(self):
        running = True
        while running:
            for event in pg.event.get():
                if event.type == pg.QUIT:  # Exit program
                    running = False
                elif event.type == pg.KEYDOWN:  # Check for key press
                    if event.key == pg.K_ESCAPE:  # Escape key pressed?
                        running = False  # Exit program

            milliseconds = self.clock.tick(self.fps)  # Update clock
            self.playtime += milliseconds / 1000.0  # Increment playtime counter
            self.draw_text("FPS: {:6.3}{}PLAYTIME: {:6.3} SECONDS".format(  # Draw text on screen
                self.clock.get_fps(), " " * 5, self.playtime))

            pg.display.flip()  # Update contents of entire display
            self.screen.blit(self.background, (0, 0))  # Draw another Surface onto current Surface, using same coord

        pg.quit()  # Uninitialize all pygame modules (only necessary to kill resources but continue)

    def draw_text(self, text):
        """Center text in window"""
        fw, fh = self.font.size(text)  # fw: font width,  fh: font height
        surface = self.font.render(text, True, (0, 255, 0))  # Create a new Surface with specified text on it
        self.screen.blit(surface, ((self.width - fw) // 2, (self.height - fh) // 2))  # Integer division to change coord



if __name__ == '__main__':
    PygView().run()  # Run program w/ default settings
