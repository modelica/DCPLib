################################################
# DCPLib test
#
# DCPLib:
# Runs one of the previously built tests to
# assert correct compilation, linking and
# correct dependency resolution.
#
################################################

COPY run_example.sh /DCPLib/run_example.sh


# Run Tests for DCPLib
WORKDIR /DCPLib
RUN chmod +x /DCPLib/run_example.sh
RUN ./run_example.sh
