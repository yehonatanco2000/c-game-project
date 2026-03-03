#include "player.h"
#include "game.h"
#include <cctype>
#include <cmath>

bool player::pickupIfPossible() {
  char object = screen.charAt(p); // Character on current tile

  if (!iscarrying_item() && object != '?' && object != '/' && object != '\\' &&
      object != ' ' && object != '#' && object != '*' && object != '+' &&
      object != '-') {
    // Hand is empty and tile is a pickup item (not riddle/switch/empty)
    setCarryItem(object);                    // Take the item
    screen.setChar(p.getY(), p.getX(), ' '); // Clear tile on board
    if (!gameptr->isSilent())
      screen.drawChar(p.getX(), p.getY(), ' ',
                      Color::WHITE_BRIGHT); // Erase from screen
    return true;                            // Now holding an item
  }

  // Hand already holds key/bomb ? swap or drop according to design
  if (getCarryItem() == '@' || getCarryItem() == 'k' ||
      getCarryItem() == '!' && !gameptr->isSilent()) {

    screen.setChar(p.getY(), p.getX(),
                   object); // Put previous object back on floor
    if (object == 'k') {
      screen.drawChar(p.getX(), p.getY(), object,
                      Color::YELLOW); // Draw key in yellow
    } else if (object == '@') {
      screen.drawChar(p.getX(), p.getY(), object,
                      Color::RED); // Draw bomb in red
    } else if (object == '!') {
      screen.drawChar(p.getX(), p.getY(), object,
                      Color::BLUE); // Draw torch in blue
    } else {
      screen.drawChar(p.getX(), p.getY(), object,
                      Color::WHITE_BRIGHT); // Default color
    }
  }
  return false; // Did not pick up (hand full)
}

void player::move() {
  if (!isactive) {
    return; // Inactive player does not move
  }
  // 1) Try spring-based movement first
  if (moveWithSpring()) {
    if (gameptr) {
      updateHUD();
    }
    return; // Do not perform normal move when spring handled it
  }
  // Compute desired next position based on current direction
  int nextX = p.getX();
  int nextY = p.getY();
  Direction d = p.getDir();
  nextX += d.dx();
  nextY += d.dy();
  point nextPoint(nextX, nextY, {0, 0}, p.c);

  // If next tile is occupied by the other player - do not move
  if (gameptr && gameptr->isPlayerAt(nextX, nextY, this)) {
    p.dir = Direction::directions[Direction::STAY]; // Stop moving
    return;
  }
  // Check HUD area
  if (gameptr && gameptr->isHudArea(nextX, nextY)) {
    p.dir = Direction::directions[Direction::STAY];
    return;
  }

  if (screen.isWall(nextPoint)) {
    p.dir = Direction::directions[Direction::STAY];
    return;
  }
  // Check obstacle
  if (screen.isobstacle(nextPoint)) {
    if (!(gameptr && gameptr->tryPushObstacle(nextPoint, d))) {
      // Cannot push
      p.x = nextX - d.dx();
      p.y = nextY - d.dy();
      if (gameptr && !gameptr->isSilent())
        p.draw();
      return;
    }
    // Obstacle moved, can continue
  }
  // Normal move
  // Restore the object at the old position instead of just drawing space
  char charAtOldPos = screen.charAt(p);
  if (charAtOldPos != ' ' && !screen.isDark(getX(), getY()) ||
      screen.isCellLitByTorch(getX(), getY())) {
    Color c = Color::WHITE_BRIGHT;
    if (charAtOldPos == '@')
      c = Color::RED;
    else if (charAtOldPos == 'k')
      c = Color::YELLOW;
    else if (charAtOldPos == '!')
      c = Color::BLUE;
    else if (charAtOldPos == '?')
      c = Color::GREEN;
    else if (charAtOldPos == '*')
      c = Color::LIGHT_GREEN;
    else if (charAtOldPos == '/' || charAtOldPos == '\\')
      c = Color::CYAN;
    else if (charAtOldPos >= '1' && charAtOldPos <= '9')
      c = Color::MAGENTA;
    else if (charAtOldPos == '#')
      c = Color::BROWN;
    if (gameptr && !gameptr->isSilent())
      screen.drawChar(p.getX(), p.getY(), charAtOldPos, c);
  } else {
    if (!gameptr->isSilent())
      p.draw(' '); // Draw space if nothing there
  }
  p.move();
  if (!gameptr->isSilent()) {
    p.draw();
  }

  if (gameptr) {
    gameptr->handleStepOnObjects(
        *this);  // React to items, doors, switches, etc.
    updateHUD(); // Update HUD to reflect any changes
    if (isactive && !gameptr->isSilent())
      p.draw(); // Ensure player is drawn on top after interactions
  }
}

void player::keyPressed(char ch) {
  size_t index = 0;                               // Index into Moves array
  for (char move : Moves) {                       // Scan all movement keys
    if (std::tolower(move) == std::tolower(ch)) { // Match ignoring case
      p.changeDir(Direction::directions[index]); // Change direction accordingly
      break;
    }
    ++index;
  }
  // Dispose item - depends on which player this is
  if ((p.c == '$' && (ch == 'e')) || // Player 1 uses 'e'
      (p.c == '&' && (ch == 'o'))) { // Player 2 uses 'o'
    disposeItem();                   // Try to drop/throw carried item
  }
}

void player::setStartPosition(int x, int y) {
  p.x = x;         // Update internal point x
  p.y = y;         // Update internal point y
  startpointX = x; // Store new starting X
  startpointY = y; // Store new starting Y
}

bool player::moveWithSpring() {
  if (springTicksLeft <= 0)
    return false; // not under spring effect

  // Base velocity from spring
  int vx = springDir.dx() * springSpeed;
  int vy = springDir.dy() * springSpeed;

  // Allow sideways movement (player input)
  Direction inputDir = p.getDir();

  int sidex = inputDir.dx();
  int sidey = inputDir.dy();

  // Forbid movement opposite to spring direction
  if (sidex == -springDir.dx() && sidex != 0)
    sidex = 0;
  if (sidey == -springDir.dy() && sidey != 0)
    sidey = 0;

  vx += sidex;
  vy += sidey;

  // Number of small steps to take this frame
  int ax = std::abs(vx);
  int ay = std::abs(vy);
  int steps = (ax > ay) ? ax : ay; // max

  int stepX = (steps > 0) ? vx / steps : 0;
  int stepY = (steps > 0) ? vy / steps : 0;

  for (int i = 0; i < steps; ++i) {
    int nextX = p.getX() + stepX;
    int nextY = p.getY() + stepY;

    // 1. Check other player
    if (gameptr && gameptr->isPlayerAt(nextX, nextY, this)) {
      // Collision under spring effect: push the other player
      player *other = gameptr->getOtherPlayer(this);

      // Give other same spring effect
      other->setSpringEffect(springSpeed, springTicksLeft, springDir);

      // We can either stop here or continue;
      clearSpringEffect();
      p.dir = Direction::directions[Direction::STAY];
      return true;
    }

    point nextPoint(nextX, nextY, {0, 0}, p.c);
    point old_pos = p;
    if (!gameptr->isSilent())
      p.draw(' '); // Original drawing

    // 2. Check HUD
    if (gameptr && gameptr->isHudArea(nextX, nextY)) {
      p = old_pos;
      if (gameptr && !gameptr->isSilent())
        p.draw();
      clearSpringEffect();
      if (gameptr)
        gameptr->handleStepOnObjects(*this);
      return true;
    }
    // 3. check wall
    if (screen.isWall(nextPoint)) {
      p = old_pos;
      if (gameptr && !gameptr->isSilent())
        p.draw();
      clearSpringEffect();
      if (gameptr)
        gameptr->handleStepOnObjects(*this);
      return true;
    }

    // 4. check obstacle
    if (screen.isobstacle(nextPoint)) {
      if (gameptr && gameptr->tryPushObstacle(nextPoint, springDir)) {
        p = old_pos;
        p.x = nextX;
        p.y = nextY;
        if (gameptr && !gameptr->isSilent())
          p.draw();
        if (gameptr)
          gameptr->handleStepOnObjects(*this);
        continue;
      } else {
        p = old_pos;
        if (gameptr && !gameptr->isSilent())
          p.draw();
        clearSpringEffect();
        if (gameptr)
          gameptr->handleStepOnObjects(*this);
        return true;
      }
    }

    // No collisions - move normally
    p.x = nextX;
    p.y = nextY;
    if (gameptr && !gameptr->isSilent())
      p.draw();
    if (gameptr)
      gameptr->handleStepOnObjects(*this);
  }

  // decrease remaining spring ticks
  --springTicksLeft;
  if (springTicksLeft == 0) {
    clearSpringEffect();
  }
  if (gameptr && !gameptr->isSilent()) {
    // Restore board characters along the spring path (fixes erased '#' springs)
    // Walk back from current position to the start, redrawing board chars
    int cx = p.getX() - vx;  // starting position before this frame's spring move
    int cy = p.getY() - vy;
    for (int i = 0; i < steps; ++i) {
      if (cx >= 0 && cx <= Screen::MAX_X && cy >= 0 && cy <= Screen::MAX_Y) {
        char boardCh = screen.board[cy][cx];
        if (boardCh != ' ') {
          // Pick correct color based on char (same as Screen::draw)
          Color col = Color::WHITE_BRIGHT;
          if (boardCh == '#') col = Color::BROWN;
          else if (boardCh == '*') col = Color::LIGHT_GREEN;
          else if (boardCh == '?' ) col = Color::GREEN;
          else if (boardCh == 'k') col = Color::YELLOW;
          else if (boardCh == '@') col = Color::RED;
          else if (boardCh == '!' ) col = Color::BLUE;
          else if (boardCh == '/' || boardCh == '\\\\') col = Color::CYAN;
          else if (boardCh == 'W') col = Color::LIGHT_CYAN;
          else if (boardCh >= '1' && boardCh <= '9') col = Color::MAGENTA;
          screen.drawChar(cx, cy, boardCh, col);
        }
      }
      cx += stepX;
      cy += stepY;
    }
    // No full screen.draw() here ׳’ג‚¬ג€ individual p.draw() calls above
    // already handle each cell. Full redraw caused visible flickering.
    gameptr->drawHUD();
    updateHUD();
  }
  return true; // we handled movement this frame
}

void player::updateHUD() {
  if (gameptr && gameptr->isSilent())
    return;

  if (!gameptr || !gameptr->isHudZone(gameptr->getHudX(), gameptr->getHudY())) {
    return;
  }

  int baseX = gameptr->getHudX();
  int baseY = gameptr->getHudY();

  int rowY = (p.c == '$') ? baseY : baseY + 1;

  int itemX = baseX + 18;
  int lifeX = baseX + 27;

  char itemCh = getCarryItem();
  if (itemCh == ' ')
    itemCh = ' ';

  Color itemColor = Color::WHITE_BRIGHT;
  if (itemCh == '@')
    itemColor = Color::RED;
  else if (itemCh == 'k')
    itemColor = Color::YELLOW;
  else if (itemCh == '!')
    itemColor = Color::BLUE;

  screen.setChar(rowY, itemX, itemCh);
  if (!gameptr->isSilent())
    screen.drawChar(itemX, rowY, itemCh, itemColor);

  char lifeCh = (life >= 0 && life <= 9) ? char('0' + life) : '?';

  screen.setChar(rowY, lifeX, lifeCh);
  if (!gameptr->isSilent())
    screen.drawChar(lifeX, rowY, lifeCh, Color::WHITE_BRIGHT);
}

void player::disposeItem() {
  if (carry_item == ' ') { // If hand is empty, nothing to dispose
    return;
  }

  int dropX = p.getX(); // Start drop position at player x
  int dropY = p.getY(); // Start drop position at player y

  Direction d = p.getDir(); // Current movement direction

  if (d.dx() != 0 || d.dy() != 0) {
    // Throw item behind the player relative to movement direction
    dropX -= d.dx();
    dropY -= d.dy();
  } else {
    // Player is not moving: search for a free nearby tile
    const int offsets[][2] = {
        {1, 0},   // Right
        {-1, 0},  // Left
        {0, -1},  // Up
        {0, 1},   // Down
        {1, -1},  // Right-Up
        {1, 1},   // Right-Down
        {-1, -1}, // Left-Up
        {-1, 1}   // Left-Down
    };

    bool found = false; // True if a free tile is found
    for (auto &off : offsets) {
      int nx = dropX + off[0]; // Candidate x
      int ny = dropY + off[1]; // Candidate y

      point candidate(nx, ny, {0, 0}, ' '); // Temporary point for checks

      if (!screen.isWall(candidate) && !screen.isInteractable(candidate)) {
        dropX = nx; // Use this free tile
        dropY = ny;
        found = true;
        break;
      }
    }

    // If no free tile is found around the player, do not drop the item
    if (!found) {
      return;
    }
  }

  point target(dropX, dropY, {0, 0}, ' '); // Final drop position
  if (screen.isWall(target) || screen.isInteractable(target)) {
    return; // Do not drop on wall/object
  }

  // Place the item on the dynamic board and draw it
  screen.setChar(dropY, dropX, carry_item); // Update board cell with item
  if (!screen.isDark(target.getX(), target.getY()) ||
      screen.isCellLitByTorch(target.getX(), target.getY())) {
    if (!gameptr->isSilent())
      screen.drawChar(dropX, dropY, carry_item, // Draw with color by item type
                      (carry_item == '@')   ? Color::RED
                      : (carry_item == 'k') ? Color::YELLOW
                      : (carry_item == '!') ? Color::BLUE
                                            : Color::WHITE_BRIGHT);
  }

  if (carry_item == '@') { // If dropped item is a bomb
    if (gameptr)
      gameptr->addBomb(dropX, dropY); // Register active bomb in game
  }
  carry_item = ' '; // Clear player's hand
}
