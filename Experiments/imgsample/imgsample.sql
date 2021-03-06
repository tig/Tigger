if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ImageDeleteCommand]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[ImageDeleteCommand]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ImageInsertCommand]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[ImageInsertCommand]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ImageSelectCommand]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[ImageSelectCommand]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ImageUpdateCommand]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[ImageUpdateCommand]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[img_SelectAll]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[img_SelectAll]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[image]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[image]
GO

if not exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[image]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
 BEGIN
CREATE TABLE [dbo].[image] (
	[img_pk] [int] IDENTITY (1, 1) NOT NULL ,
	[img_name] [varchar] (256) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[img_desc] [varchar] (1024) COLLATE SQL_Latin1_General_CP1_CI_AS NULL ,
	[img_dir] [varchar] (256) COLLATE SQL_Latin1_General_CP1_CI_AS NULL ,
	[img_full] [image] NULL ,
	[img_med] [image] NULL ,
	[img_thumb] [image] NULL ,
	[img_contenttype] [varchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NULL ,
	[img_datepicturetaken] [datetime] NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
END

GO

ALTER TABLE [dbo].[image] ADD 
	CONSTRAINT [PK_image] PRIMARY KEY  NONCLUSTERED 
	(
		[img_pk]
	)  ON [PRIMARY] ,
	CONSTRAINT [IX_image] UNIQUE  NONCLUSTERED 
	(
		[img_name]
	)  ON [PRIMARY] ,
	CONSTRAINT [CK_image_name_valid] CHECK (left([img_name],1) <> ' ' and right([img_name],1) <> ' ' and charindex('\',ltrim(rtrim([img_name]))) = 0 and charindex('/',ltrim(rtrim([img_name]))) = 0 and charindex('?',ltrim(rtrim([img_name]))) = 0 and charindex('&',ltrim(rtrim([img_name]))) = 0 and charindex('*',ltrim(rtrim([img_name]))) = 0 and charindex('..',[img_name]) = 0)
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE  PROCEDURE dbo.ImageDeleteCommand
(
	@Original_img_pk int,
	@Original_img_contenttype varchar(50),
	@Original_img_datepicturetaken datetime,
	@Original_img_desc varchar(1024),
	@Original_img_dir varchar(256),
	@Original_img_name varchar(256)
)
AS
	SET NOCOUNT OFF;
DELETE FROM image WHERE (img_pk = @Original_img_pk) AND (img_contenttype = @Original_img_contenttype OR @Original_img_contenttype IS NULL AND img_contenttype IS NULL) AND (img_datepicturetaken = @Original_img_datepicturetaken OR @Original_img_datepicturetaken IS NULL AND img_datepicturetaken IS NULL) AND (img_desc = @Original_img_desc OR @Original_img_desc IS NULL AND img_desc IS NULL) AND (img_dir = @Original_img_dir OR @Original_img_dir IS NULL AND img_dir IS NULL) AND (img_name = @Original_img_name)

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE  PROCEDURE dbo.ImageInsertCommand
(
	@img_name varchar(256),
	@img_desc varchar(1024),
	@img_dir varchar(256),
	@img_full image,
	@img_med image,
	@img_thumb image,
	@img_contenttype varchar(50),
	@img_datepicturetaken datetime
)
AS
	SET NOCOUNT OFF;
INSERT INTO image(img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken) VALUES (@img_name, @img_desc, @img_dir, @img_full, @img_med, @img_thumb, @img_contenttype, @img_datepicturetaken);
	SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image WHERE (img_pk = @@IDENTITY)

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE  PROCEDURE dbo.ImageSelectCommand
AS
	SET NOCOUNT ON;
SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE  PROCEDURE dbo.ImageUpdateCommand
(
	@img_name varchar(256),
	@img_desc varchar(1024),
	@img_dir varchar(256),
	@img_full image,
	@img_med image,
	@img_thumb image,
	@img_contenttype varchar(50),
	@img_datepicturetaken datetime,
	@Original_img_pk int,
	@Original_img_contenttype varchar(50),
	@Original_img_datepicturetaken datetime,
	@Original_img_desc varchar(1024),
	@Original_img_dir varchar(256),
	@Original_img_name varchar(256),
	@img_pk int
)
AS
	SET NOCOUNT OFF;
UPDATE image SET img_name = @img_name, img_desc = @img_desc, img_dir = @img_dir, img_full = @img_full, img_med = @img_med, img_thumb = @img_thumb, img_contenttype = @img_contenttype, img_datepicturetaken = @img_datepicturetaken WHERE (img_pk = @Original_img_pk) AND (img_contenttype = @Original_img_contenttype OR @Original_img_contenttype IS NULL AND img_contenttype IS NULL) AND (img_datepicturetaken = @Original_img_datepicturetaken OR @Original_img_datepicturetaken IS NULL AND img_datepicturetaken IS NULL) AND (img_desc = @Original_img_desc OR @Original_img_desc IS NULL AND img_desc IS NULL) AND (img_dir = @Original_img_dir OR @Original_img_dir IS NULL AND img_dir IS NULL) AND (img_name = @Original_img_name);
	SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image WHERE (img_pk = @img_pk)

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

CREATE PROCEDURE dbo.img_SelectAll
@orderby VARCHAR(30) = 'img_name' -- This is the input parameter.
AS
	SET NOCOUNT ON;

SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype
FROM [image] 
ORDER BY 
	CASE @orderby
		 --WHEN 'img_pk' THEN [img_pk]
         WHEN 'img_name' THEN [img_name]
         WHEN 'img_desc' THEN [img_desc]
         WHEN 'img_dir' THEN [img_dir]
         WHEN 'img_contenttype' THEN [img_contenttype]
         ELSE null
	END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

