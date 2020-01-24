#pragma once

class WsError
{
    WS_ERROR* m_h;

public:

    WsError() :
        m_h(0)
    {
        // Do nothing
    }

    ~WsError()
    {
        if (0 != m_h)
        {
            WsFreeError(m_h);
        }
    }

    HRESULT Create(__in_ecount_opt(propertyCount) const WS_ERROR_PROPERTY* properties, 
                   __in ULONG propertyCount)
    {
        ASSERT(0 == m_h);

        return WsCreateError(properties,
                             propertyCount,
                             &m_h);
    }

    HRESULT GetProperty(__in WS_ERROR_PROPERTY_ID id, 
                        __out_bcount(bufferSize) void* buffer, 
                        __in ULONG bufferSize)
    {
        ASSERT(0 != m_h);
        ASSERT(0 != buffer);

        return WsGetErrorProperty(m_h,
                                  id,
                                  buffer,
                                  bufferSize);
    }

    template <typename T>
    HRESULT GetProperty(__in WS_ERROR_PROPERTY_ID id,
                        __out T* buffer)
    {
        return GetProperty(id,
                           buffer,
                           sizeof(T));
    }

    HRESULT GetString(__in ULONG index, 
                      __out WS_STRING* string)
    {
        ASSERT(0 != m_h);
        ASSERT(0 != string);

        return WsGetErrorString(m_h,
                                index,
                                string);
    }

    operator WS_ERROR*() const
    {
        return m_h;
    }
};

class WsHeap
{
    WS_HEAP* m_h;

public:

    WsHeap() :
        m_h(0)
    {
        // Do nothing
    }

    ~WsHeap()
    {
        if (0 != m_h)
        {
            WsFreeHeap(m_h);
        }
    }

    HRESULT Create(__in SIZE_T maxSize, 
                   __in SIZE_T trimSize, 
                   __in_opt const WS_HEAP_PROPERTY* properties, 
                   __in ULONG propertyCount, 
                   __in_opt WS_ERROR* error)
    {
        ASSERT(0 == m_h);

        return WsCreateHeap(maxSize,
                            trimSize,
                            properties,
                            propertyCount,
                            &m_h,
                            error);
    }

    operator WS_HEAP*() const
    {
        return m_h;
    }

};

class WsServiceProxy
{
    WS_SERVICE_PROXY* m_h;

public:

    WsServiceProxy() :
        m_h(0)
    {
        // Do nothing
    }

    ~WsServiceProxy()
    {
        if (0 != m_h)
        {
            Close(0, // async context
                  0); // error

            WsFreeServiceProxy(m_h);
        }
    }

    HRESULT Open(__in const WS_ENDPOINT_ADDRESS* address, 
                 __in_opt const WS_ASYNC_CONTEXT* asyncContext, 
                 __in_opt WS_ERROR* error)
    {
        ASSERT(0 != m_h);

        return WsOpenServiceProxy(m_h,
                                  address,
                                  asyncContext,
                                  error);
    }

    HRESULT Close(__in_opt const WS_ASYNC_CONTEXT* asyncContext, 
                  __in_opt WS_ERROR* error)
    {
        ASSERT(0 != m_h);

        return WsCloseServiceProxy(m_h,
                                   asyncContext,
                                   error);
    }

    WS_SERVICE_PROXY** operator&()
    {
        ASSERT(0 == m_h);
        return &m_h;
    }

    operator WS_SERVICE_PROXY*() const
    {
        return m_h;
    }

};

class WsServiceHost
{
    WS_SERVICE_HOST* m_h;

public:

    WsServiceHost() :
        m_h(0)
    {
        // Do nothing
    }

    ~WsServiceHost()
    {
        if (0 != m_h)
        {
            Close(0, // async context
                  0); // error

            WsFreeServiceHost(m_h);
        }
    }

    HRESULT Create(__in_ecount_opt(endpointCount) const WS_SERVICE_ENDPOINT** endpoints, 
                   __in const USHORT endpointCount, 
                   __in_ecount_opt(servicePropertyCount) const WS_SERVICE_PROPERTY* properties, 
                   __in ULONG propertyCount, 
                   __in_opt WS_ERROR* error)
    {
        ASSERT(0 == m_h);

        return WsCreateServiceHost(endpoints,
                                   endpointCount,
                                   properties,
                                   propertyCount,
                                   &m_h,
                                   error);
    }

    HRESULT Open(__in_opt const WS_ASYNC_CONTEXT* asyncContext, 
                 __in_opt WS_ERROR* error)
    {
        ASSERT(0 != m_h);

        return WsOpenServiceHost(m_h,
                                 asyncContext,
                                 error);
    }

    HRESULT Close(__in_opt const WS_ASYNC_CONTEXT* asyncContext, 
                  __in_opt WS_ERROR* error)
    {
        ASSERT(0 != m_h);

        return WsCloseServiceHost(m_h,
                                  asyncContext,
                                  error);
    }

    operator WS_SERVICE_HOST*() const
    {
        return m_h;
    }
};
