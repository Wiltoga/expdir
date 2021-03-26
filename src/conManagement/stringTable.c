#include "stringTable.h"
#include <string.h>
#include <stdio.h>
#include "stringAnsiManagement.h"

void sort(int **list, size_t size)
{
    int swapped;
    do
    {
        swapped = 0;
        for (int i = 0; i < size - 1; ++i)
        {
            if (*list[i] > *list[i + 1])
            {
                swapped = 1;
                int *tmp = list[i];
                list[i] = list[i + 1];
                list[i + 1] = tmp;
            }
        }
        --size;
    } while (swapped);
}

size_t table_render(table_t table, int offsetX, int offsetY, void *buffer)
{
    uint64_t init = (uint64_t)buffer;
    int *VContentSize = (int *)malloc(sizeof(int) * table.CellsCountV);
    int *HContentSize = (int *)malloc(sizeof(int) * table.CellsCountH);
    int *VCellSize = (int *)malloc(sizeof(int) * table.CellsCountV);
    int *HCellSize = (int *)malloc(sizeof(int) * table.CellsCountH);
    //minimum sizes
    for (int i = 0; i < table.nCells; ++i)
    {
        int max = 0;
        for (int j = 0; j < table.cells[i].contentLines; ++j)
        {
            size_t len = table.cells[i].contentSize[j];
            if (max < len)
                max = len;
        }
        if (HCellSize[table.cells[i].posX] < max)
            HCellSize[table.cells[i].posX] = max;
        if (VCellSize[table.cells[i].posY] < table.cells[i].contentLines)
            VCellSize[table.cells[i].posY] = table.cells[i].contentLines;
    }
    memcpy(HContentSize, HCellSize, sizeof(int) * table.CellsCountH);
    memcpy(VContentSize, VCellSize, sizeof(int) * table.CellsCountV);
    //minimum sizes set
    {
        //setting actual size H
        int width = table.CellsCountH + 1;
        for (int i = 0; i < table.CellsCountH; ++i)
            width += HCellSize[i];
        int delta = (int)table.minWidth - width;
        if (delta > 0)
        {
            int bonus = delta / table.CellsCountH;
            int top = delta - (bonus * table.CellsCountH);
            for (int i = 0; i < table.CellsCountH; ++i)
                HCellSize[i] += bonus;
            if (top > 0)
            {
                int **sorted = (int **)malloc(sizeof(int *) * table.CellsCountH);
                for (int i = 0; i < table.CellsCountH; ++i)
                    sorted[i] = &HCellSize[i];
                sort(sorted, table.CellsCountH);
                for (int i = 0; i < top; ++i)
                    ++*sorted[table.CellsCountH - i - 1];
                free(sorted);
            }
        }
    }
    {
        //setting actual size V
        int height = table.CellsCountV + 1;
        for (int i = 0; i < table.CellsCountV; ++i)
            height += VCellSize[i];
        int delta = (int)table.minHeight - height;
        if (delta > 0)
        {
            int bonus = delta / table.CellsCountV;
            int top = delta - (bonus * table.CellsCountV);
            for (int i = 0; i < table.CellsCountV; ++i)
                VCellSize[i] += bonus;
            if (top > 0)
            {
                int **sorted = (int **)malloc(sizeof(int *) * table.CellsCountV);
                for (int i = 0; i < table.CellsCountV; ++i)
                    sorted[i] = &VCellSize[i];
                sort(sorted, table.CellsCountV);
                for (int i = 0; i < top; ++i)
                    ++*sorted[table.CellsCountV - i - 1];
                free(sorted);
            }
        }
    }
    //actual sizes set
    char *straight_H;
    char *straight_V;
    char *corner_tl;
    char *corner_tr;
    char *corner_bl;
    char *corner_br;
    char *cross;
    char *Tjunction_l;
    char *Tjunction_r;
    char *Tjunction_t;
    char *Tjunction_b;
    switch (table.tableLinesType)
    {
    case TABLE_LINES_BASIC:
        straight_H = "-";
        straight_V = "|";
        corner_tl = "+";
        corner_tr = "+";
        corner_bl = "+";
        corner_br = "+";
        cross = "+";
        Tjunction_l = "+";
        Tjunction_r = "+";
        Tjunction_t = "+";
        Tjunction_b = "+";
        break;
    case TABLE_LINES_SINGLE:
        straight_H = "─";
        straight_V = "│";
        corner_tl = "┌";
        corner_tr = "┐";
        corner_bl = "└";
        corner_br = "┘";
        cross = "┼";
        Tjunction_l = "┤";
        Tjunction_r = "├";
        Tjunction_t = "┴";
        Tjunction_b = "┬";
        break;
    case TABLE_LINES_DOUBLE:
        straight_H = "═";
        straight_V = "║";
        corner_tl = "╔";
        corner_tr = "╗";
        corner_bl = "╚";
        corner_br = "╝";
        cross = "╬";
        Tjunction_l = "╣";
        Tjunction_r = "╠";
        Tjunction_t = "╩";
        Tjunction_b = "╦";
        break;
    case TABLE_LINES_NONE:
        straight_H = " ";
        straight_V = " ";
        corner_tl = " ";
        corner_tr = " ";
        corner_bl = " ";
        corner_br = " ";
        cross = " ";
        Tjunction_l = " ";
        Tjunction_r = " ";
        Tjunction_t = " ";
        Tjunction_b = " ";
        break;

    default:
        break;
    }
    //drawing box
    int totalWidth = table.CellsCountH + 1;
    for (int i = 0; i < table.CellsCountH; ++i)
        totalWidth += HCellSize[i];
    int totalHeight = table.CellsCountV + 1;
    for (int i = 0; i < table.CellsCountV; ++i)
        totalHeight += VCellSize[i];
    buffer += string_setCursorPosition(buffer, offsetX, offsetY);
    buffer += sprintf(buffer, "%s", corner_tl);
    for (int i = 0; i < table.CellsCountH; ++i)
    {
        for (int j = 0; j < HCellSize[i]; ++j)
            buffer += sprintf(buffer, "%s", straight_H);
        if (i < table.CellsCountH - 1)
        {
            if (table.titleLeftSize > i)
                buffer += sprintf(buffer, "%s", Tjunction_b);
            else
                buffer += sprintf(buffer, "%s", straight_H);
        }
    }
    buffer += sprintf(buffer, "%s", corner_tr);
    buffer += string_setCursorPosition(buffer, offsetX, offsetY + totalHeight - 1);
    buffer += sprintf(buffer, "%s", corner_bl);
    for (int i = 0; i < table.CellsCountH; ++i)
    {
        for (int j = 0; j < HCellSize[i]; ++j)
            buffer += sprintf(buffer, "%s", straight_H);
        if (i < table.CellsCountH - 1)
        {
            if (table.titleLeftSize > i)
                buffer += sprintf(buffer, "%s", Tjunction_t);
            else
                buffer += sprintf(buffer, "%s", straight_H);
        }
    }
    buffer += sprintf(buffer, "%s", corner_br);
    //outside drawn
    //drawing inside
    {
        int y = offsetY + 1;
        for (int i = 0; i < table.CellsCountV; ++i)
        {
            for (int j = 0; j < VCellSize[i]; ++j)
            {
                int x = offsetX;
                buffer += string_setCursorPosition(buffer, x, y);
                buffer += sprintf(buffer, "%s", straight_V);
                x++;
                for (int k = 0; k < table.CellsCountH; ++k)
                {
                    x += HCellSize[k];
                    buffer += string_setCursorPosition(buffer, x, y);
                    if (table.titleLeftSize > k || k == table.CellsCountH - 1)
                        buffer += sprintf(buffer, "%s", straight_V);
                    x++;
                }
                y++;
            }
            if (i < table.CellsCountV - 1)
            {
                buffer += string_setCursorPosition(buffer, offsetX, y);
                if (table.titleTopSize > i)
                    buffer += sprintf(buffer, "%s", Tjunction_r);
                else
                    buffer += sprintf(buffer, "%s", straight_V);
                for (int k = 0; k < table.CellsCountH; ++k)
                {
                    if (table.titleTopSize > i)
                        for (int j = 0; j < HCellSize[k]; ++j)
                            buffer += sprintf(buffer, "%s", straight_H);
                    else
                        for (int j = 0; j < HCellSize[k]; ++j)
                            buffer += sprintf(buffer, " ");
                    if (table.titleLeftSize > k && k < table.CellsCountH - 1)
                    {
                        if (table.titleTopSize > i)
                            buffer += sprintf(buffer, "%s", cross);
                        else
                            buffer += sprintf(buffer, "%s", straight_V);
                    }
                    else if (k == table.CellsCountH - 1)
                    {
                        if (table.titleTopSize > i)
                            buffer += sprintf(buffer, "%s", Tjunction_l);
                        else
                            buffer += sprintf(buffer, "%s", straight_V);
                    }
                    else
                    {
                        if (table.titleTopSize > i)
                            buffer += sprintf(buffer, "%s", straight_H);
                        else
                            buffer += sprintf(buffer, " ");
                    }
                }
            }
            y++;
        }
    }
    //inside drawn
    //drawing cells
    {
        for (int i = 0; i < table.nCells; ++i)
        {
            tableCell_t *cell = &table.cells[i];
            int top = offsetY + 1 + cell->posY;
            int left = offsetX + 1 + cell->posX;
            for (int j = 0; j < cell->posY; ++j)
                top += VCellSize[j];
            for (int j = 0; j < cell->posX; ++j)
                left += HCellSize[j];
            //topleft cell set
            if (!(cell->alignement & CELL_ALIGNEMENT_TOP))
            {
                int delta = VCellSize[cell->posY] - (int)cell->contentLines;
                if (cell->alignement & CELL_ALIGNEMENT_BOT)
                    top += delta;
                else
                    top += delta / 2;
                //top content set
            }
            for (int j = 0; j < cell->contentLines; ++j)
            {
                int lineLeft = left;
                if (!(cell->alignement & CELL_ALIGNEMENT_LEFT))
                {
                    int delta = HCellSize[cell->posX] - (int)cell->contentSize[j];
                    if (cell->alignement & CELL_ALIGNEMENT_RIGHT)
                        lineLeft += delta;
                    else
                        lineLeft += delta / 2;
                    //left content set
                }
                buffer += string_setCursorPosition(buffer, lineLeft, top + j);
                buffer += sprintf(buffer, "%s", cell->content[j]);
            }
        }
    }
    free(VCellSize);
    free(HCellSize);
    buffer += string_setCursorPosition(buffer, 1, offsetY + totalHeight);
    *(char *)buffer = '\0';
    buffer += sizeof(char);
    return (uint64_t)buffer - init;
}