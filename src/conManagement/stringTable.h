#ifndef DEF_STRING_TABLE_H
#define DEF_STRING_TABLE_H
#include <stdlib.h>
#include <stdint.h>

#define TABLE_LINES_BASIC 0
#define TABLE_LINES_SINGLE 1
#define TABLE_LINES_DOUBLE 2
#define TABLE_LINES_NONE 3

#define CELL_ALIGNEMENT_TOP 0b1 << 0
#define CELL_ALIGNEMENT_CENTERV 0b1 << 1
#define CELL_ALIGNEMENT_BOT 0b1 << 2
#define CELL_ALIGNEMENT_LEFT 0b1 << 3
#define CELL_ALIGNEMENT_CENTERH 0b1 << 4
#define CELL_ALIGNEMENT_RIGHT 0b1 << 5
#define CELL_ALIGNEMENT_TOP_LEFT CELL_ALIGNEMENT_TOP | CELL_ALIGNEMENT_LEFT
#define CELL_ALIGNEMENT_TOP_CENTER CELL_ALIGNEMENT_TOP | CELL_ALIGNEMENT_CENTERH
#define CELL_ALIGNEMENT_TOP_RIGHT CELL_ALIGNEMENT_TOP | CELL_ALIGNEMENT_RIGHT
#define CELL_ALIGNEMENT_CENTER_LEFT CELL_ALIGNEMENT_CENTERV | CELL_ALIGNEMENT_LEFT
#define CELL_ALIGNEMENT_CENTER CELL_ALIGNEMENT_CENTERV | CELL_ALIGNEMENT_CENTERH
#define CELL_ALIGNEMENT_CENTER_RIGHT CELL_ALIGNEMENT_CENTERV | CELL_ALIGNEMENT_RIGHT
#define CELL_ALIGNEMENT_BOT_LEFT CELL_ALIGNEMENT_BOT | CELL_ALIGNEMENT_LEFT
#define CELL_ALIGNEMENT_BOT_CENTER CELL_ALIGNEMENT_BOT | CELL_ALIGNEMENT_CENTERH
#define CELL_ALIGNEMENT_BOT_RIGHT CELL_ALIGNEMENT_BOT | CELL_ALIGNEMENT_RIGHT
#define CELL_ALIGNEMENT_MIDDLE CELL_ALIGNEMENT_CENTER
/**
 * @brief  A cell of the table
 */
typedef struct
{
    /**
     * @brief  Lines of content in the cell
     */
    char **content;
    /**
     * @brief  the number of lines in the content
     */
    size_t contentLines;
    /**
     * @brief  the size of each line in the content. Note that the content will still be displayed entirely. This parameter is only for the purpose of avoiding ANSI and specials chars in the calculations.
     */
    size_t *contentSize;
    /**
     * @brief  X index of the cell
     */
    int posX;
    /**
     * @brief  Y index of the cell
     */
    int posY;
    /**
     * @brief  How the content will be displayed
     */
    uint8_t alignement;
} tableCell_t;

typedef struct
{
    /**
     * @brief  the list of cells
     */
    tableCell_t *cells;
    /**
     * @brief  the number of cells registered
     */
    size_t nCells;
    /**
     * @brief  the width of the table in cells
     */
    size_t CellsCountH;
    /**
     * @brief  the height of the table in cells
     */
    size_t CellsCountV;
    /**
     * @brief  the min width of the table in characters
     */
    size_t minWidth;
    /**
     * @brief  the min height of the table in characters
     */
    size_t minHeight;
    /**
     * @brief  the type of borders of the table
     */
    uint8_t tableLinesType;
    /**
     * @brief  how much cells from the top are considered "title"
     */
    size_t titleTopSize;
    /**
     * @brief  how much cells from the left are considered "title"
     */
    size_t titleLeftSize;
} table_t;

/**
 * @brief  Renders the table to the given buffer
 * @param  table data of the table to render
 * @param  offsetX used to localize the left point of the table in the console
 * @param  offsetY used to localize the top point of the table in the console
 * @param  buffer buffer storing the table string data. Must be large enough
 * @return the size of the buffer used by the table
 */
size_t table_render(table_t table, int offsetX, int offsetY, void *buffer);

#endif
