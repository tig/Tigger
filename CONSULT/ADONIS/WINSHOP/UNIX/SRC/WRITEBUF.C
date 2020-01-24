#ifdef DEBUG
int
WriteBuf(char *szFileName, char *buf, int NumBytes)
{
    int fp= -1;
    extern int _fmode;
    int nRetval=OK;

    _fmode = O_BINARY;
    if ((fp = _lopen(szFileName,READ_WRITE)) == -1)
        END(FAIL)

    _llseek(fp,0L,2); // to end of file for append

    if (_lwrite(fp,buf,NumBytes) != NumBytes)
        END(FAIL)

    end:
    if (fp != -1)
        _lclose(fp);
    _fmode = O_TEXT;
    return nRetval;
}
#endif
