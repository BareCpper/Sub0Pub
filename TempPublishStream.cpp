
        /** Register a sink to the specified type buffer and completion notifier
         * @remark Called by sub0::ForwardPublish<Data>
         *
         * @param buffer  Data structure to read into
         * @param notify  Notifier object to signal buffer completion
         */
        template<typename Data>
        void addSink( Data* buffer, ISinkNotify* notify )
        {
            const SinkRecord entry = { sub0::Broker<Data>::typeId()
                                    , sizeof(*buffer)
                                    , reinterpret_cast<uint8_t*>(buffer)
                                    , notify };
            addSink( entry );
        }

    private:
        /** Data record sink data
         *
         */
        struct SinkRecord
        {
            uint32_t typeId; ///< Value of Broker<Data>::typeId() @todo Use handle to broker?
            uint32_t bufferBytes; ///< Size of buffer in bytes for validating header
            uint8_t* buffer; ///< Buffer to write data into
            ISinkNotify* notify; ///< Instance to notify on buffer completion to trigger publish to occur
        };
        
    void PublishStream::addSink( const SinkRecord& entry )
    {
        mSinkEntries.push_back( entry );

        // Sort entries by typeId
        std::sort( mSinkEntries.begin(), mSinkEntries.end()
                , [](const SinkRecord& lhs, const SinkRecord& rhs) { return lhs.typeId < rhs.typeId; } );
    }

    PublishStream::SinkRecord* PublishStream::findSink( const uint32_t typeId )
    {
        // Find entry by typeId
        RecordArray::iterator iFind = std::lower_bound( mSinkEntries.begin(), mSinkEntries.end()
                , typeId
                , [](const SinkRecord& lhs, const uint32_t rhs) { return lhs.typeId < rhs; } );

        const bool sinkIsFound = (iFind != mSinkEntries.end()) && (iFind->typeId == typeId);

        return sinkIsFound ? &*iFind
                           : nullptr;
    }

    bool PublishStream::readHeader()
    {
        assert( mPacketByteCount < sizeof(mHeader) );

        char* const headBegin = reinterpret_cast<char*>(&mHeader);

        // Read header while there is data in the stream
        while ( (mPacketByteCount < sizeof(mHeader)) && mStream.good() )
        {
            mPacketByteCount += static_cast<uint32_t>(mStream.readsome( headBegin + mPacketByteCount, sizeof(mHeader) - mPacketByteCount ));
        }

        //Check if the header read was complete
        const bool headerReceived = (mPacketByteCount == sizeof(mHeader));
        if ( !headerReceived )
        {
            return false;
        }

        /// @todo Does not handle malformed data streams
        assert( mHeader.magic ==  mHeader.cMagic );
        mPayload = findSink( mHeader.typeId );

        /// @todo Does not handle discarding typeId we don't have handler for
        return true;
    }

    bool PublishStream::readPayload()
    {
        assert( (mPayload!= nullptr) && (mPayload->buffer != nullptr) ); /// Validate header/payload initial state
        assert( mPacketByteCount < (sizeof(mHeader) + mHeader.dataBytes) );

        char* const payloadBegin = reinterpret_cast<char*>(&mHeader);

        // Read payload while there is data in the stream
        while ( (mPacketByteCount < (sizeof(mHeader) + mHeader.dataBytes)) && mStream.good() )
        {
            mPacketByteCount += static_cast<uint32_t>(mStream.readsome( reinterpret_cast<char*>(mPayload->buffer + (mPacketByteCount- sizeof(mHeader)))
                                                                      , sizeof(mHeader) - mPacketByteCount ));
        }

        return mPacketByteCount == (sizeof(mHeader) + mHeader.dataBytes);
    }

    bool PublishStream::update()
    {
        const bool isReadingHeader = (mPacketByteCount < sizeof(mHeader));
        if ( mPacketByteCount < sizeof(mHeader) ) //< Is reading header
        {
            readHeader();
        }

        if ( mPacketByteCount >= sizeof(mHeader) ) // Is reading payload
        {
            readPayload();
        }

        // Payload incomplete
        const bool packetComplete = (mPacketByteCount == (sizeof(mHeader) + mHeader.dataBytes));
        if ( !packetComplete )
        {
            return false;
        }

        // Completed reading packet
        char deliminator;
        mStream.get( deliminator );
        assert( deliminator == '\n' ); ///< @todo Handle not reading deliminator!
        mPayload->notify->sinkFull(); // Signal completion to publish data signal
        mPacketByteCount = 0; // reset for next packet
        mPayload = nullptr; // reset for next packet
        return true;
    }
