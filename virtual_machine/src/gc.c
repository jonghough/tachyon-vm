#include "gc.h"
#include "memory.h"
#include "log.h"

void mark_data(data *d)
{
    if (!d)
    {
        ty_log(TY_ERROR, "bad memory found !\n");
        exit(1);
    }
    if (!d->marked || 1)
    {
        d->marked = 1;
        if (d->type == STR)
        {
            d->content.xstr->marked = 1;
        }
        else if (d->type == PTR)
        {
            d->content.xptr->marked = 1;
            for (uint32_t j = 0; j < d->content.xptr->len; j++)
            {
                data *cd = &d->content.xptr->objs[j];
                mark_data(cd);
            }
        }
        else if (d->type == REC)
        {
            d->content.xrec->marked = 1;
            for (uint32_t j = 0; j < d->content.xrec->len; j++)
            {
                data *cd = &d->content.xrec->objs[j];
                mark_data(cd);
            }
        }
        else if (d->type == FILEPTR)
        {
            d->content.xfileptr->marked = 1;
        }
    }
}

void mark(virtual_machine *v)
{
    for (uint32_t i = 0; i <= v->ST; i++)
    {
        data *d = &v->stack[i];
        mark_data(d);
    }
}

void sweep(virtual_machine *v)
{
    gc_obj *c = v->heap_refs;
    while (c)
    {
        gc_obj *n = c->next;
        if (c->type == GCDATA)
        {
            if (!c->content.xd->marked)
            {
                // DELETE
                destroy_data(c->content.xd);

                if (c->next)
                    c->next->prev = c->prev;
                if (c->prev)
                    c->prev->next = c->next;
                if (c == v->heap_refs)
                    v->heap_refs = c->next;
                free(c);
            }
            else
            {
                c->content.xd->marked = 0;
            }
        }
        else if (c->type == GCARR)
        {
            if (!c->content.xptr->marked)
            {
                destroy_parr(c->content.xptr);
                // DELETE
                if (c->next)
                    c->next->prev = c->prev;
                if (c->prev)
                    c->prev->next = c->next;
                if (c == v->heap_refs)
                    v->heap_refs = c->next;
                free(c);
            }
            else
            {
                c->content.xptr->marked = 0;
            }
        }
        else if (c->type == GCSTR)
        {
            if (!c->content.xstr->marked)
            {
                // DELETE
                destroy_pmem(c->content.xstr);
                if (c->next)
                    c->next->prev = c->prev;
                if (c->prev)
                    c->prev->next = c->next;
                if (c == v->heap_refs)
                    v->heap_refs = c->next;
                free(c);
            }
            else
            {
                c->content.xstr->marked = 0;
            }
        }
        else if (c->type == GCREC)
        {
            if (!c->content.xrec->marked)
            {
                // DELETE
                destroy_prec(c->content.xrec);
                if (c->next)
                    c->next->prev = c->prev;
                if (c->prev)
                    c->prev->next = c->next;
                if (c == v->heap_refs)
                    v->heap_refs = c->next;

                free(c);
            }
            else
            {
                c->content.xrec->marked = 0;
            }
        }
        else if (c->type == GCFILEPTR)
        {
            if (!c->content.xfileptr->marked)
            {
                // DELETE
                if (c->next)
                    c->next->prev = c->prev;
                if (c->prev)
                    c->prev->next = c->next;
                if (c == v->heap_refs)
                    v->heap_refs = c->next;
                free(c);
            }

            else
            {
                c->content.xfileptr->marked = 0;
            }
        }
        c = n;
    }
}

void run_gc(virtual_machine *v)
{
#if DEBUG_BUILD
    ty_log(TY_INFO, "**** BEGIN GC *****\n");
#endif
    mark(v);
    sweep(v);
#if DEBUG_BUILD
    ty_log(TY_INFO, "**** END GC *****\n");
#endif
}

void cleanup_gc(virtual_machine *v)
{

    gc_obj *c = v->heap_refs;
    while (c)
    {
        gc_obj *n = c->next;
        free(c);
        c = n;
    }
}
