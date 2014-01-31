package com.ford.syncV4.proxy.converter;

import junit.framework.TestCase;

import static org.hamcrest.CoreMatchers.instanceOf;
import static org.hamcrest.CoreMatchers.nullValue;
import static org.hamcrest.CoreMatchers.sameInstance;
import static org.hamcrest.MatcherAssert.assertThat;

/**
 * Tests for SyncRPCRequestConverterFactory class.
 *
 * Created by enikolsky on 2014-01-21.
 */
public class SyncRPCRequestConverterFactoryTest extends TestCase {
    private SyncRPCRequestConverterFactory factory;

    @Override
    public void setUp() throws Exception {
        super.setUp();

        factory = new SyncRPCRequestConverterFactory();
    }

    public void testGetConverterShouldReturnNullForUnknownName() {
        assertThat(factory.getConverterForFunctionName("AwesomeRequest"),
                nullValue());
    }

    public void testGetConverterShouldReturnDefaultConverterForKnownName() {
        assertThat(factory.getConverterForFunctionName("Show"),
                instanceOf(DefaultRPCRequestConverter.class));
    }

    public void testGetConverterShouldReturnDefaultConverterForPutFile() {
        assertThat(factory.getConverterForFunctionName("PutFile"),
                instanceOf(DefaultRPCRequestConverter.class));
    }

    public void testGetConverterShouldCacheConverterForTheSameName() {
        final String functionName = "Show";
        final IRPCRequestConverter converter =
                factory.getConverterForFunctionName(functionName);
        assertThat(factory.getConverterForFunctionName(functionName),
                sameInstance(converter));
    }

    public void testGetConverterShouldReturnNullWhenNameIsNull() {
        assertThat(factory.getConverterForFunctionName(null), nullValue());
    }
}
