
# rtsp


all:
%::
	@$(MAKE) --no-print-directory -C   BasicUsageEnvironment $@
	@$(MAKE) --no-print-directory -C   groupsock $@
	@$(MAKE) --no-print-directory -C   liveMedia $@
	@$(MAKE) --no-print-directory -C   UsageEnvironment $@
aclean:
	$(MAKE) -C BasicUsageEnvironment clean
	$(MAKE) -C groupsock clean
	$(MAKE) -C liveMedia clean
	$(MAKE) -C UsageEnvironment clean
