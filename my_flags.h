#pragma once
struct FLAGS
{
	volatile bool PROGRAM_TERMINATE = false;
	volatile bool PREVIEW_TERMINATE = false;
};
struct StreamFlags
{
	volatile bool STREAMING_TERMINATE = true;
	volatile bool STREAMING_SAVING = false;
};