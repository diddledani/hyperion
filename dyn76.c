/* DYN76.C (c) Copyright Harold Grovesteen, 2010-2011                 */ 
   the Compatibility Parameter Block, which is in essence a R0-R15 register 
   save area.  These macros ensure storage keys and accesses are checked for 
   the parameter block.
    
/* Compile with debugging */
#define DYN76_DEBUG
    int  handle;          /* Host file system handle */
/* File Keeper list.  New entries are added at the top of the list */
static struct fkeeper *fkpr_head = NULL;  /* open file status list */
static struct fkeeper *rst_head = NULL;   /* Restart list */
static U32 restart_id = 0;
#if 0
# endif
    fk = fkpr_head;      /* Search the list */
            unlock (nfile_lock);
#if 0
    pfk = NULL;          /* previous entry */
    fk = fkpr_head;      /* Current entry, starting with the head */
        /* Set up for next cycle */
        pfk = fk;      /* current entry becomes the previous */
        fk = fk->next; /* current entry's next, becomes the current */
#endif
        {   /* Found the entry */
    int    space_ctl;           /* This is used to control address space selection */
    int    handle = 0;          /* Host file file handle for this file */
    U32    ghandle = 0;         /* Guest file descriptor */
    struct fkeeper *fk = NULL;  /* Host file structure */
    struct fkeeper *rfk = NULL; /* Restart structure */
    
    U32    R0;
    U32    R1;
    U32    R2;
    U32    R3;
    U32    R4;
    U32    R5;
    U32    R15;
#ifdef DYN76_DEBUG
    LOGMSG("DF18: CFILE Validating FOCPB Address %X\n", cmpb);
#endif
    /* CPB must be on a doubleword and must not cross physical page boundary */
    if ( ((cmpb & 0x7) != 0 ) ||
         (((cmpb + 63) & STORAGE_KEY_PAGEMASK) != (cmpb & STORAGE_KEY_PAGEMASK)) )
        ARCH_DEP(program_interrupt) (regs, PGM_SPECIFICATION_EXCEPTION);
        
#ifdef DYN76_DEBUG
    LOGMSG("DF18: CFILE Validated FOCPB Address\n");
#endif

    get_reg(R1, 1);     /* Retrieve the function number */
    {   /* Invalid Function - generate an exception */
        set_reg(0,R0);  /* don't restart this */
    
    get_reg(R2, 2);  /* All functions use parameter 1, so fetch it */
    
    /* read, write, seek, commit, close and setmode use the file descriptor */
    if (R1 >= 4 && R1 <= 9)
    {
        if (R1 == 4 || R1 == 5) 
        {   /* For read and write guest file descriptor is in pseudo R3 */ 
            get_reg(R3, 3);
            ghandle = R3;
        } 
        else
        {   /* For seek, commit, close and setmode, file descriptor is in
               previously fetched pseudo R2 */
            ghandle = R2;
        }
        
        /* If read, write, seek, commit, close or setmode */
        /* convert the file descriptor into a file handle */
        fk = FindFK(ghandle);
        if (!fk)
        {   /* Did not find the file descriptor - generate an exception */
            
#ifdef DYN76_DEBUG
            LOGMSG("DF18: CFILE - guest file descriptor not found: %d\n", ghandle);
#endif
            R0 = 0;
            set_reg(0,R0); /* No restart on a failure */
            ARCH_DEP(program_interrupt) (regs, PGM_OPERAND_EXCEPTION);
        }
        handle = fk->handle;  /* All host file accesses use this variable */
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - host file handle: %d\n", handle);
#endif
    }

/* The following 4 functions are always ready to attempt 
   and they are not interruptible: CLOSE, COMMIT, SEEK, SETMODE
*/

    /*------------------------*/
    /* SETMODE File Operation */
    /*------------------------*/
    
    {   
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - setmode file operation\n");
#endif
        get_reg(R3, 3);   /* Fetch parameter two - new file mode */
        i = _setmode (handle, i); /* Alter the Windows mode */
        /* *nix doesn't need handle updates */
        if (fk->mode)
            fk->mode = 1;    /* yes, translate */
            fk->mode = 0;    /* no, dont */
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - close file operation\n");
#endif
        R15 = _close (handle);
        {   RemoveFKByID (fk->id);
        }
        {   R15 = -errno;
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - commit file operation\n");
#endif

        R15 =  _commit (handle);
        R15 = fsync (handle);
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - seek file operation\n");
#endif
        R15 = _lseek (handle, R3, R4);
    get_reg(R0,0);        /* Retrieve Restart Stage */
    {   /* New operation, not a restart.  Establish new operational state */
        rfk = malloc (sizeof (struct fkeeper));
        if (rfk == NULL) 
        rfk->mode = -1;      /* mode is not initially set */
        rfk->data = 0;       /* Nothing in the buffer yet */
        rfk->handle = 0;     /* No file handle yet either (could be an open) */
        get_reg(R5,5);       /* Save pseudo R5 */
        rfk->SaveArea = R5;
           must be removed in stage 3
        dolock (nfile_lock);    /* Take ownership of the list */
        R5 = nfile_id++;        /* safely increment the id counter */
        rfk->id = R5;
        rfk->next = fkpr_head;  /* Add to the top of the list */
        fkpr_head = rfk;        /* New potential entry becomes the first */
        unlock (nfile_lock);    /* Release ownership of the list */
        set_reg(5,R5);          /* Set the restart state for this new operation */
        /* For read/write we need this cleared here */
        /* Set the restart stage in case the next stage is interrupted */
        R0 = 1;                /* Set work stage 1 on restart */
        rfk = FindFK(R5);
        if (!rfk)
            while ((rfk->data == 0) ||                    /* Started? */
                   (rfk->oldname [rfk->data - 1] != 0)) 
                /* WARNING: This is where interruption might occur */
                    (&(fk->oldname [rfk->data]), 
                rfk->data += 1;  /* Next host byte location */
                if (rfk->data >= 259) 
                    rfk->oldname [fk->data] = 0;
            StrConverter (rfk->oldname, DCCebcdic_to_ascii);
            rfk->data = 0; /* Get ready for newname */
            while ((rfk->data == 0)                        /* Starting */ 
                   || (rfk->filename [rfk->data - 1] != 0)) /* Not Finished */
                /* WARNING: This is where interruption might occur */
                    (&(rfk->filename [rfk->data]), 
                rfk->data += 1;  /* Next host byte location */
                if (rfk->data >= 259) 
                    rfk->filename [rfk->data] = 0;
            StrConverter (rfk->filename, DCCebcdic_to_ascii);
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - rename file operation\n");
#endif
        R5 = rfk->SaveArea;
        R15 = rename (rfk->oldname, rfk->filename);
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - unlink file operation\n");
#endif
        R5 = rfk->SaveArea;
        R15 = _unlink (rfk->filename);
        R15 = remove (rfk->filename);
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - Open operation\n");
#endif
        R5 = rfk->SaveArea;
        /* Convert to host platform native open flags */
            rfk->mode = 1; /* Record the desire to translate codepages */
            rfk->mode = 0; /* Binary mode (untranslated) */
            R4 = 0666;  /* Note octal value */
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - Opening file: %s\n", rfk->filename);
#endif
        R15 = _open (rfk->filename, i, R4);
        {   /* Successful host file open */
            
            /* Save the handle for use in other operations */
            rfk->handle = R15;  /* Save the host handle */
            /* This restart ID becomes the guest file descriptor */
            R15 = rfk->id;
            set_reg(15,R15);
#ifdef DYN76_DEBUG
            LOGMSG("DF18: CFILE - opened host file handle: %d\n", rfk->handle);
            LOGMSG("DF18: CFILE - opened guest file descriptor: %d\n", rfk->id);
            rfk = NULL;                   
            /* Note: during the start of the interruptable open operation the
               fkeeper structure was linked to the fkeeper list to allow restart 
               of the operation from the restart id.  Now that we have been 
               successful in opening the file, we will leave the restart fkeeper
               on the list as the link to the host file from the guest.
            */
        {   /* Failed host file open */
            
            /* Note: during start of the interruptable open operation the 
               fkeeper structure was linked to the fkeeper list to allow restart 
               of the operation from the restart id.  On a failure to actually open 
               the file, the structure needs to be removed from the list and that
               will happen below just before returning to hdiagf18.c
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - Orphan close operation\n");
#endif
        R5 = rfk->SaveArea;
        R15 = RemoveFKByName (rfk->filename);
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - read file operation\n");
#endif
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - read requested bytes: %d\n", R4);
#endif
            if (rfk->data == 0) { /* Need to fill our buffer with some data? */
                /*get_reg(R3,3); */
                rfk->data = _read (handle, rfk->filename, i);
#ifdef DYN76_DEBUG
                LOGMSG("DF18: CFILE - host read result: %d\n", rfk->data);
#endif
                    MemConverter (rfk->filename, DCCascii_to_ebcdic, rfk->data);
            if (rfk->data < 0)
            if (rfk->data <= 0)
            i = rfk->data - 1;
            
            /* WARNING: This is where interruption might occur */
                (rfk->filename,  /* This member is used as a buffer */
            rfk->data = 0;
        R5 = rfk->SaveArea;
#ifdef DYN76_DEBUG
        LOGMSG("DF18: CFILE - write file operation\n");
#endif
            
            /* WARNING: This is where interruption might occur */
                (rfk->filename, 
                MemConverter (rfk->filename, DCCebcdic_to_ascii, i);
                /* rfk->filename is being used as a data buffer here */
            i = _write (handle, rfk->filename, i);
    if (rfk) /* clean up, unless open already has */
        RemoveFKByID (rfk->id);  /* Safely remove from the restart state */
    
    set_reg(0,R0);